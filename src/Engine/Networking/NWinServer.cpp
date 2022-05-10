// Folder: Networking

#include "EnginePCH.h"
#include "NWinServer.h"

#ifdef BP_PLATFORM_WINDOWS

bool NWinServer::m_IsInitialized = false;

NWinServer::NWinServer()
	: m_IsShuttingDown(false)
	, m_IoCompletionPortThreadCount(0)
	, m_IoCompletionPortHandle(nullptr)
	, m_Socket(INVALID_SOCKET)
{
	if (!m_IsInitialized)
	{
		WSADATA wsaData;
		int status = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (status != 0)
		{
			LOG_ERROR("Failed to initialize Winsock, error_code=%d", status);
			return;
		}

		m_IsInitialized = true;
	}
}

NWinServer::~NWinServer()
{
	m_IsShuttingDown = true;

	for (uint i = 0; i < MAX_THREAD_COUNT; ++i)
	{
		PostQueuedCompletionStatus(m_IoCompletionPortHandle, 0, 0, nullptr);
	}

	if (m_IoCompletionPortHandle != INVALID_HANDLE_VALUE && m_IoCompletionPortHandle != NULL)
		CloseHandle(m_IoCompletionPortHandle);

	for (uint i = 0; i < MAX_THREAD_COUNT; ++i)
	{
		if (m_IoCompletionPortThreads[i].joinable())
			m_IoCompletionPortThreads[i].join();
	}

	if (m_Socket != INVALID_SOCKET && m_Socket != NULL)
		closesocket(m_Socket);

	if (m_MainAcceptingThread.joinable())
		m_MainAcceptingThread.join();

	for (const std::pair<SOCKET, PerSocketContext*>& pair : m_Connections)
	{
		CloseClientSocket(pair.first, false);
	}

	while (!m_pPerIoContextPool.empty())
	{
		SAFE_DELETE(m_pPerIoContextPool.front());
		m_pPerIoContextPool.pop();
	}
}

void NWinServer::StartListening(uint port, bool shouldUseAcceptThread)
{
	if (m_PacketValidator == nullptr)
	{
		LOG_ERROR("Packet validator needs to be set before starting to listen on server, call SetPacketValidator()");
		return;
	}

	if (m_ReplyCallback == nullptr)
	{
		LOG_ERROR("Reply callback needs to be set before starting to listen on server, call SetReplyCallback()");
		return;
	}

	addrinfo info;
	ZeroMemory(&info, sizeof(info));
	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_STREAM;
	info.ai_protocol = IPPROTO_TCP;
	info.ai_flags = AI_PASSIVE;

	const std::string portStr = std::to_string(port);
	addrinfo* pResultInfo;
	int status = getaddrinfo(NULL, portStr.c_str(), &info, &pResultInfo);
	if (status != 0)
	{
		LOG_ERROR("Failed to resolve local ip address for port=%d, error_code=%d", port, status);
		return;
	}

	m_Socket = WSASocket(pResultInfo->ai_family, pResultInfo->ai_socktype, pResultInfo->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_Socket == INVALID_SOCKET)
	{
		LOG_ERROR("Failed to create server socket, error_code=%d", WSAGetLastError());
		freeaddrinfo(pResultInfo);
		return;
	}

	status = bind(m_Socket, pResultInfo->ai_addr, (int)pResultInfo->ai_addrlen);
	freeaddrinfo(pResultInfo);
	if (status != 0)
	{
		LOG_ERROR("Failed to bind server socket, error_code=%d", status);
		closesocket(m_Socket);
		return;
	}

	status = listen(m_Socket, SOMAXCONN);
	if (status != 0)
	{
		LOG_ERROR("Failed to start listening on server socket, error_code=%d", status);
		closesocket(m_Socket);
		return;
	}

	m_IoCompletionPortHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_IoCompletionPortHandle == NULL)
	{
		LOG_ERROR("Failed to create I/O completion port for server, error_code=%d", GetLastError());
		return;
	}

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	m_IoCompletionPortThreadCount = std::clamp((uint)systemInfo.dwNumberOfProcessors * 2, 0u, MAX_THREAD_COUNT);

	for (uint i = 0; i < m_IoCompletionPortThreadCount; ++i)
	{
		m_IoCompletionPortThreads[i] = std::thread(IOCompletionPortWorker, this);
	}

	if (shouldUseAcceptThread)
		m_MainAcceptingThread = std::thread(IOCompletionPortMain, this);
	else
		IOCompletionPortMain(this);
}

// This function will only be executed by a single thread, accepting connections from clients
void NWinServer::IOCompletionPortMain(NWinServer* pInstance)
{
	while (!pInstance->m_IsShuttingDown)
	{
		SOCKET clientSocket = WSAAccept(pInstance->m_Socket, NULL, NULL, ConnectionAcceptCondition, 0);
		if (clientSocket == INVALID_SOCKET && !pInstance->m_IsShuttingDown)
		{
			uint errorCode = WSAGetLastError();
			if (errorCode == WSAECONNREFUSED)
			{
				LOG_DEBUG("Refused client connection as the connecting IP is blacklisted");
				continue;
			}

			LOG_ERROR("Failed to accept client socket connection, error_code=%d", errorCode);
			continue;
		}

		if (pInstance->m_IsShuttingDown)
			break;

		sockaddr_in addressData;
		int addressSize = sizeof(addressData);
		uint errCode = getpeername(clientSocket, (sockaddr*)&addressData, &addressSize) != 0;
		if (errCode != 0)
		{
			LOG_ERROR("Failed to retrieve the socket address, error_code=%d", errCode);
			closesocket(clientSocket);
			continue;
		}

		char readableAddressBuffer[INET_ADDRSTRLEN];
		if (inet_ntop(addressData.sin_family, &addressData.sin_addr, readableAddressBuffer, sizeof(readableAddressBuffer)) == NULL)
		{
			LOG_ERROR("Failed to convert address to readable address, error_code=%d", WSAGetLastError());
			closesocket(clientSocket);
			continue;
		}

		PerSocketContext* pSocketContext = new PerSocketContext();
		pSocketContext->clientSocket = clientSocket;
		pSocketContext->address = addressData.sin_addr;
		pSocketContext->readableAddress = readableAddressBuffer;
		PerIoContext* pIoContext = pInstance->AcquirePerIoContext(pSocketContext);
		pIoContext->operation = CompletedOperation::ClientRead;
		pIoContext->wsaBuffer.buf = pIoContext->buffer;
		pIoContext->wsaBuffer.len = sizeof(pIoContext->buffer);

		std::unique_lock lock(pInstance->m_ConnectionsMutex);
		{
			NetworkUUID uuid(pSocketContext->address.S_un.S_addr);
			while (pInstance->m_ConnectedUUIDsCache.find(uuid) != pInstance->m_ConnectedUUIDsCache.end())
			{
				uuid = NetworkUUID(pSocketContext->address.S_un.S_addr);
			}

			pSocketContext->uuid = uuid;

			pInstance->m_Connections.emplace(clientSocket, pSocketContext);
			pInstance->m_ConnectedUUIDsCache.emplace(uuid, clientSocket);
		}

		lock.unlock();

		HANDLE completionPortHandle = CreateIoCompletionPort((HANDLE)clientSocket, pInstance->m_IoCompletionPortHandle, (ULONG_PTR)pSocketContext, 0);
		if (completionPortHandle != pInstance->m_IoCompletionPortHandle)
		{
			LOG_ERROR("CreateIoCompletionPort returned a different handle than the one passed as argument");
			return;
		}

		DWORD flags = 0;
		int status = WSARecv(clientSocket, &pIoContext->wsaBuffer, 1, NULL, &flags, &pIoContext->overlapped, NULL);
		if (status != 0)
		{
			int errorCode = WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				LOG_ERROR("Failed to receive data from client socket, return_code=%d, error_code=%d", status, errorCode);
				pInstance->ReleasePerIoContextAndCloseSocket(pIoContext, pSocketContext);
				continue;
			}
		}
	}
}

// This function will be executed by the left over threads (m_ThreadCount - 1), handling I/O completion port packets
void NWinServer::IOCompletionPortWorker(NWinServer* pInstance)
{
	while (!pInstance->m_IsShuttingDown)
	{
		DWORD nBytesTransferred = 0;
		PerSocketContext* pSocketContext = nullptr;
		WSAOVERLAPPED* pOverlapped = nullptr;

		BOOL success = GetQueuedCompletionStatus(pInstance->m_IoCompletionPortHandle, &nBytesTransferred, (PULONG_PTR)&pSocketContext, (LPOVERLAPPED*)&pOverlapped, INFINITE);

		if (pInstance->m_IsShuttingDown)
			break;

		PerIoContext* pIoContext = (PerIoContext*)pOverlapped;

		if (!success)
		{
			const DWORD errorCode = GetLastError();
			const long completionPortHandleClosedError = ERROR_ABANDONED_WAIT_0;
			const long clientSocketClosedError = ERROR_NETNAME_DELETED;

			if (errorCode != completionPortHandleClosedError && errorCode != clientSocketClosedError)
				LOG_ERROR("GetQueuedCompletionStatus failed with error_code=%d", errorCode);

			if (errorCode == clientSocketClosedError)
				pInstance->ReleasePerIoContextAndCloseSocket(pIoContext, pSocketContext);

			continue;
		}

		if (pSocketContext == nullptr)
		{
			LOG_ERROR("GetQueuedCompletionStatus has nullptr as pSocketContext");
			continue;
		}

		if (nBytesTransferred == 0)
		{
			// Client closed the socket
			pInstance->ReleasePerIoContextAndCloseSocket(pIoContext, pSocketContext);
			continue;
		}

		switch (pIoContext->operation)
		{
		case CompletedOperation::ClientRead:
		{
			// Reading - reconstructing the packet(s)
			ULONG nextPacketPosition = 0;
			while (nextPacketPosition < nBytesTransferred)
			{
				PerIoContext* pNewIoContext = pInstance->AcquirePerIoContext(pSocketContext);
				if (pNewIoContext == nullptr)
					break;

				pNewIoContext->wsaBuffer.buf = pNewIoContext->buffer;
				pNewIoContext->wsaBuffer.len = sizeof(pNewIoContext->buffer);

				pNewIoContext->packet.Reset();
				pNewIoContext->packet.ConstructPacket(pIoContext->wsaBuffer.buf + nextPacketPosition, std::min(nBytesTransferred - nextPacketPosition, (ULONG)NPacket::MAX_PACKET_SIZE));
				pNewIoContext->packet.clientUUID = pSocketContext->uuid;
				pNewIoContext->packet.address = pSocketContext->address.S_un.S_addr;
				pNewIoContext->packet.readableAddress = pSocketContext->readableAddress;
				if (pNewIoContext->packet.identifier == INVALID_PACKET_IDENTIFIER)
				{
					LOG_ERROR("Received network packet resulted in an invalid packet during parsing, dropped the packet");
					break;
				}

				uint packetSize = 0;
				if (!pInstance->m_PacketValidator(pNewIoContext->packet, packetSize))
				{
					LOG_ERROR("Received network packet didn't pass the validation check, dropped the packet");
					break;
				}

				pNewIoContext->packet.data.SetWriteBufferPointer(packetSize);
				nextPacketPosition += packetSize + NPacket::MAX_HEADER_SIZE;

				// Finished reading a single packet - writing a response
				pInstance->m_ReplyCallback(pNewIoContext->packet);
				pNewIoContext->operation = CompletedOperation::ClientWrite;
				pNewIoContext->totalBytes = (uint)pNewIoContext->packet.data.GetWriteBufferPointer() + NPacket::MAX_HEADER_SIZE;

				ByteStreamBufferImpl<NPacket::MAX_BODY_SIZE>* pPacketBufferImpl = static_cast<ByteStreamBufferImpl<NPacket::MAX_BODY_SIZE>*>(pNewIoContext->packet.data.GetImpl());

				pNewIoContext->wsaBuffer.len = pNewIoContext->totalBytes;
				std::copy((CHAR*)&pNewIoContext->packet.identifier, ((CHAR*)&pNewIoContext->packet.identifier) + NPacket::MAX_HEADER_SIZE, pNewIoContext->wsaBuffer.buf);
				std::copy(pPacketBufferImpl->GetBuffer(), pPacketBufferImpl->GetBuffer() + pNewIoContext->packet.data.GetWriteBufferPointer(), (std::byte*)(pNewIoContext->wsaBuffer.buf + NPacket::MAX_HEADER_SIZE));

				int sendStatus = WSASend(pSocketContext->clientSocket, &pNewIoContext->wsaBuffer, 1, NULL, 0, &pNewIoContext->overlapped, NULL);
				if (sendStatus != 0)
				{
					int errorCode = WSAGetLastError();
					if (errorCode != WSA_IO_PENDING)
					{
						LOG_ERROR("Failed to send data to client socket, return_code=%d, error_code=%d", sendStatus, errorCode);
						pInstance->ReleasePerIoContextAndCloseSocket(pNewIoContext, pSocketContext);
						continue;
					}
				}
			}

			// Reading again, doesn't wait until the packet(s) have finished sending
			pIoContext->operation = CompletedOperation::ClientRead;
			pIoContext->wsaBuffer.len = sizeof(pIoContext->buffer);

			DWORD flags = 0;
			int readStatus = WSARecv(pSocketContext->clientSocket, &pIoContext->wsaBuffer, 1, NULL, &flags, &pIoContext->overlapped, NULL);
			if (readStatus != 0)
			{
				int errorCode = WSAGetLastError();
				if (errorCode != WSA_IO_PENDING)
				{
					LOG_ERROR("Failed to receive data from client socket, return_code=%d, error_code=%d", readStatus, errorCode);
					pInstance->ReleasePerIoContextAndCloseSocket(pIoContext, pSocketContext);
					continue;
				}
			}

			break;
		}
		case CompletedOperation::ClientWrite:
		{
			// Finished writing a response - checking to make sure all data was sent
			if (pIoContext->totalBytes != pIoContext->wsaBuffer.len)
			{
				LOG_ERROR("Byte count we were trying to send does not match to the actual sent byte count, trying_send=%d, actual_send=%d", pIoContext->totalBytes, pIoContext->wsaBuffer.len);
				continue;
			}

			pInstance->ReleasePerIoContext(pIoContext, pSocketContext);
			break;
		}
		}
	}
}

int NWinServer::ConnectionAcceptCondition(LPWSABUF lpCallerId, LPWSABUF, LPQOS, LPQOS, LPWSABUF, LPWSABUF, GROUP*, DWORD_PTR)
{
	// No caller id means no way to get the IP before connecting, just reject
	if (lpCallerId == nullptr)
	{
		LOG_ERROR("Connection does not have caller id, no way to determine IP address before accepting the connection, rejecting");
		return CF_REJECT;
	}

	// Connection is not IPV4, this should never happen, so reject
	if (((sockaddr*)lpCallerId->buf)->sa_family != AF_INET)
	{
		LOG_ERROR("Connection is not IPV4 address, rejecting");
		return CF_REJECT;
	}

	// IP is blacklisted, reject
	sockaddr_in* pClientAddress = (sockaddr_in*)lpCallerId->buf;
	if (m_BlacklistedIPs.find((uint)pClientAddress->sin_addr.S_un.S_addr) != m_BlacklistedIPs.end())
		return CF_REJECT;

	return CF_ACCEPT;
}

void NWinServer::CloseClient(const NetworkUUID& uuid)
{
	SOCKET clientSocket = INVALID_SOCKET;

	{
		std::unique_lock lock(m_ConnectionsMutex);

		if (!IsClientConnected(uuid))
			return;

		clientSocket = m_ConnectedUUIDsCache.at(uuid);
	}

	closesocket(clientSocket);
}

void NWinServer::CloseClientSocket(SOCKET clientSocket, bool shouldEraseFromConnections, bool shouldDeleteSocketContext)
{
	if (clientSocket == NULL || clientSocket == INVALID_SOCKET)
		return;

	PerSocketContext* pContext;
	{
		std::unique_lock lock(m_ConnectionsMutex);

		if (m_Connections.find(clientSocket) == m_Connections.end())
		{
			LOG_WARNING("Client socket not found in connections list when trying to close");
			closesocket(clientSocket);
			return;
		}

		pContext = m_Connections[clientSocket];

		if (shouldEraseFromConnections)
		{
			m_Connections.erase(clientSocket);
			m_ConnectedUUIDsCache.erase(pContext->uuid);
		}
	}

	if (m_ClientDroppedCallback != nullptr)
		m_ClientDroppedCallback(pContext->uuid);

	{
		PerIoContext* pNextIoContext = pContext->pPerIoContextListFirst;
		while (pNextIoContext != nullptr)
		{
			PerIoContext* pTemp = pNextIoContext;
			pNextIoContext = pNextIoContext->pNextPerIoContext;
			SAFE_DELETE(pTemp);
		}

		if (shouldDeleteSocketContext)
			SAFE_DELETE(pContext);
	}

	closesocket(clientSocket);
}

NWinServer::PerIoContext* NWinServer::AcquirePerIoContext()
{
	std::unique_lock lock(m_PerIoContextPoolMutex);
	if (!m_pPerIoContextPool.empty())
	{
		PerIoContext* pContext = m_pPerIoContextPool.front();
		m_pPerIoContextPool.pop();
		return pContext;
	}
	lock.unlock();

	return new PerIoContext();
}

NWinServer::PerIoContext* NWinServer::AcquirePerIoContext(PerSocketContext* pSocketContext)
{
	std::scoped_lock lock(pSocketContext->perIoContextListMutex);

	if (pSocketContext->clientSocket == NULL || pSocketContext->clientSocket == INVALID_SOCKET)
		return nullptr;

	PerIoContext* pNewContext = AcquirePerIoContext();
	pNewContext->pPreviousPerIoContext = nullptr;
	pNewContext->pNextPerIoContext = nullptr;

	PerIoContext* pLastIoContext = pSocketContext->pPerIoContextListLast;
	if (pLastIoContext == nullptr)
	{
		pSocketContext->pPerIoContextListFirst = pNewContext;
		pSocketContext->pPerIoContextListLast = pNewContext;
	}
	else
	{
		pLastIoContext->pNextPerIoContext = pNewContext;
		pNewContext->pPreviousPerIoContext = pLastIoContext;
		pSocketContext->pPerIoContextListLast = pNewContext;
	}

	return pNewContext;
}

void NWinServer::ReleasePerIoContext(PerIoContext* pContext)
{
	std::scoped_lock lock(m_PerIoContextPoolMutex);
	m_pPerIoContextPool.push(pContext);
}

void NWinServer::ReleasePerIoContext(PerIoContext* pContext, PerSocketContext* pSocketContext)
{
	std::scoped_lock lock(pSocketContext->perIoContextListMutex);
	ReleasePerIoContextWithoutLock(pContext, pSocketContext);
}

void NWinServer::ReleasePerIoContextWithoutLock(PerIoContext* pContext, PerSocketContext* pSocketContext)
{
	if (pContext->pPreviousPerIoContext != nullptr)
		pContext->pPreviousPerIoContext->pNextPerIoContext = pContext->pNextPerIoContext;
	else
		pSocketContext->pPerIoContextListFirst = pContext->pNextPerIoContext;

	if (pContext->pNextPerIoContext != nullptr)
		pContext->pNextPerIoContext->pPreviousPerIoContext = pContext->pPreviousPerIoContext;
	else
		pSocketContext->pPerIoContextListLast = pContext->pPreviousPerIoContext;

	ReleasePerIoContext(pContext);
}

void NWinServer::ReleasePerIoContextAndCloseSocket(PerIoContext* pIoContext, PerSocketContext* pSocketContext)
{
	std::unique_lock lock(pSocketContext->perIoContextListMutex);

	// Releasing and closing socket
	ReleasePerIoContextWithoutLock(pIoContext, pSocketContext);
	closesocket(pSocketContext->clientSocket);

	// Full clean up, including PerSocketContext
	if (pSocketContext->pPerIoContextListFirst == nullptr && pSocketContext->pPerIoContextListLast == nullptr)
	{
		CloseClientSocket(pSocketContext->clientSocket, true, false);
		pSocketContext->clientSocket = INVALID_SOCKET;

		lock.unlock();
		SAFE_DELETE(pSocketContext);
	}
}

#endif