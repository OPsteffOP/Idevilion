// Folder: Networking

#pragma once
#include "NClient.h"

#ifdef BP_PLATFORM_WINDOWS

class NWinClient : public NClient
{
public:
	NWinClient();
	virtual ~NWinClient() override;

	virtual void Connect(const std::string& address, uint port) override;
	virtual void CloseConnection() override;

	virtual void SendPacket(const NPacket& packet) override;
	virtual NPacket ReadPacket(void* pUserData = nullptr) override;

	virtual bool IsConnected() const override;

protected:
	void CloseConnectionAndWarn();

protected:
	static bool m_IsInitialized;

	SOCKET m_Socket;
};

#endif