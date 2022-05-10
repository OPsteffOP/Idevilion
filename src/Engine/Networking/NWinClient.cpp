// Folder: Networking

#include "EnginePCH.h"
#include "NWinClient.h"

#ifdef BP_PLATFORM_WINDOWS

bool NWinClient::m_IsInitialized = false;

NWinClient::NWinClient()
	: m_Socket(INVALID_SOCKET)
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

NWinClient::~NWinClient()
{
	if (m_Socket != INVALID_SOCKET)
		closesocket(m_Socket);
}

void NWinClient::Connect(const std::string& address, uint port)
{
	addrinfo info;
	ZeroMemory(&info, sizeof(info));
	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_STREAM;
	info.ai_protocol = IPPROTO_TCP;

	const std::string portStr = std::to_string(port);
	addrinfo* pResultInfo;
	int status = getaddrinfo(address.c_str(), portStr.c_str(), &info, &pResultInfo);
	if (status != 0)
	{
		LOG_ERROR("Failed to resolve ip address from server: %s:%d, error_code=%d", address.c_str(), port, status);
		return;
	}

	m_Socket = socket(pResultInfo->ai_family, pResultInfo->ai_socktype, pResultInfo->ai_protocol);
	if (m_Socket == INVALID_SOCKET)
	{
		LOG_ERROR("Failed to create client socket to connect to server: %s:%d, error_code=%d", address.c_str(), port, WSAGetLastError());
		freeaddrinfo(pResultInfo);
		return;
	}

	addrinfo* pCurrentInfo = pResultInfo;
	while (connect(m_Socket, pCurrentInfo->ai_addr, (int)pCurrentInfo->ai_addrlen) != 0)
	{
		pCurrentInfo = pCurrentInfo->ai_next;
		if (pCurrentInfo == nullptr)
			break;
	}
	const bool connected = pCurrentInfo != nullptr;

	freeaddrinfo(pResultInfo);
	if (!connected)
	{
		LOG_ERROR("Failed to connect to server: %s:%d, error_code=%d", address.c_str(), port, WSAGetLastError());
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return;
	}
}

void NWinClient::CloseConnection()
{
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}

void NWinClient::CloseConnectionAndWarn()
{
	CloseConnection();

	if (m_ConnectionDiedCallback != nullptr)
		m_ConnectionDiedCallback();
}

void NWinClient::SendPacket(const NPacket& packet)
{
	if (m_Socket == INVALID_SOCKET)
	{
		LOG_ERROR("Trying to send a packet to an invalid socket, check if the connected is closed");
		return;
	}

	ByteStreamBufferImpl<NPacket::MAX_BODY_SIZE>* pPacketBufferImpl = static_cast<ByteStreamBufferImpl<NPacket::MAX_BODY_SIZE>*>(packet.data.GetImpl());

	std::byte tempSendBuffer[NPacket::MAX_PACKET_SIZE];
	std::copy((std::byte*)&packet.identifier, (std::byte*)&packet.identifier + NPacket::MAX_HEADER_SIZE, tempSendBuffer);
	std::copy(pPacketBufferImpl->GetBuffer(), pPacketBufferImpl->GetBuffer() + packet.data.GetWriteBufferPointer(), tempSendBuffer + NPacket::MAX_HEADER_SIZE);

	int nSentBytes = send(m_Socket, reinterpret_cast<const char*>(tempSendBuffer), NPacket::MAX_HEADER_SIZE + (uint)packet.data.GetWriteBufferPointer(), 0);
	if (nSentBytes == SOCKET_ERROR)
	{
		LOG_ERROR("Failed to send data through a socket, error_code=%d", WSAGetLastError());
		CloseConnectionAndWarn();
		return;
	}
}

NPacket NWinClient::ReadPacket(void* pUserData)
{
	if (m_Socket == INVALID_SOCKET)
	{
		LOG_ERROR("Trying to read a packet from an invalid socket, check if the connection is closed");
		return NPacket();
	}

	char receiveBuffer[NPacket::MAX_PACKET_SIZE];
	NPacket receivePacket;

	int nReceivedBytes = recv(m_Socket, receiveBuffer, sizeof(receiveBuffer), 0);
	if (nReceivedBytes == SOCKET_ERROR)
	{
		LOG_ERROR("Failed to receive data through a socket, error_code=%d", WSAGetLastError());
		CloseConnectionAndWarn();
		return NPacket();
	}

	receivePacket.Reset();
	receivePacket.ConstructPacket(receiveBuffer, nReceivedBytes);
	receivePacket.pUserData = pUserData;

	return receivePacket;
}

bool NWinClient::IsConnected() const
{
	return m_Socket != INVALID_SOCKET;
}

#endif