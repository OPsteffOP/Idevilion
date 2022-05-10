// Folder: Networking

#pragma once
#include "NPacket.h"

class NClient
{
public:
	typedef std::function<void()> ConnectionDiedCallback;

public:
	NClient();
	virtual ~NClient() = default;

	virtual void Connect(const std::string& address, uint port) = 0;
	virtual void CloseConnection() = 0;

	virtual void SendPacket(const NPacket& packet) = 0;
	virtual NPacket ReadPacket(void* pUserData = nullptr) = 0;

	virtual bool IsConnected() const = 0;

	void SetConnectionDiedCallback(ConnectionDiedCallback callback) { m_ConnectionDiedCallback = callback; }

protected:
	ConnectionDiedCallback m_ConnectionDiedCallback;
};