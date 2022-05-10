// Folder: Networking

#pragma once
#include "NServer.h"

#ifdef BP_PLATFORM_WINDOWS

#include <thread>

class NWinServer : public NServer
{
public:
	NWinServer();
	virtual ~NWinServer() override;

	virtual void StartListening(uint port, bool shouldUseAcceptThread = true) override;

	virtual bool IsClientConnected(const NetworkUUID& uuid) const override { return m_ConnectedUUIDsCache.find(uuid) != m_ConnectedUUIDsCache.end(); }
	virtual void CloseClient(const NetworkUUID& uuid) override;

protected:
	static void IOCompletionPortMain(NWinServer* pInstance);
	static void IOCompletionPortWorker(NWinServer* pInstance);
	static int ConnectionAcceptCondition(LPWSABUF lpCallerId, LPWSABUF, LPQOS, LPQOS, LPWSABUF, LPWSABUF, GROUP*, DWORD_PTR);

	void CloseClientSocket(SOCKET clientSocket, bool shouldEraseFromConnections = true, bool shouldDeleteSocketContext = true);

protected:
	static bool m_IsInitialized;

	bool m_IsShuttingDown;

	constexpr static const uint MAX_THREAD_COUNT = 16;
	uint m_IoCompletionPortThreadCount;
	std::thread m_IoCompletionPortThreads[MAX_THREAD_COUNT];

	HANDLE m_IoCompletionPortHandle;
	SOCKET m_Socket;

	std::thread m_MainAcceptingThread;

private:
	enum class CompletedOperation
	{
		ClientRead,
		ClientWrite
	};

	struct PerIoContext
	{
		WSAOVERLAPPED overlapped;
		CompletedOperation operation;

		char buffer[NPacket::MAX_PACKET_SIZE];
		WSABUF wsaBuffer;
		uint totalBytes;

		NPacket packet;

		PerIoContext* pPreviousPerIoContext;
		PerIoContext* pNextPerIoContext;
	};

	struct PerSocketContext
	{
		SOCKET clientSocket;
		NetworkUUID uuid;

		in_addr address;
		std::string readableAddress;

		std::mutex perIoContextListMutex;
		PerIoContext* pPerIoContextListFirst;
		PerIoContext* pPerIoContextListLast;
	};

private:
	PerIoContext* AcquirePerIoContext();
	PerIoContext* AcquirePerIoContext(PerSocketContext* pSocketContext);
	void ReleasePerIoContext(PerIoContext* pContext);
	void ReleasePerIoContext(PerIoContext* pContext, PerSocketContext* pSocketContext);

	void ReleasePerIoContextWithoutLock(PerIoContext* pContext, PerSocketContext* pSocketContext);
	void ReleasePerIoContextAndCloseSocket(PerIoContext* pIoContext, PerSocketContext* pSocketContext);

private:
	std::mutex m_PerIoContextPoolMutex;
	std::queue<PerIoContext*> m_pPerIoContextPool;

	std::mutex m_ConnectionsMutex;
	std::unordered_map<SOCKET, PerSocketContext*> m_Connections;
	std::unordered_map<NetworkUUID, SOCKET, NetworkUUID::Hasher> m_ConnectedUUIDsCache;
};

#endif