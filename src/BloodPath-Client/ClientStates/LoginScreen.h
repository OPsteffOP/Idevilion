// Folder: ClientStates

#pragma once
#include "Game.h"

#include "ClientVersion.h"
#include "NFactory.h"
#include "GamePackets.h"
#include "NPacket.h"

class NClient;

class LoginScreen : public Game
{
public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Render() override;

	const std::string& GetWorldServerIP() const { return m_WorldServerIP; }
	uint GetWorldServerPort() const { return m_WorldServerPort; }
	const NetworkUUID& GetSessionToken() const { return m_SessionToken; }

private:
	void ServerConnectionDied(NClient*& pClient);

	UserAuthenticationStatus Login(NClient* pClient, const std::string& login, const std::string& password);

private:
	constexpr static const char* LOGIN_SERVER_HOST = "localhost";
	constexpr static const uint LOGIN_SERVER_PORT = 1234;

	std::string m_WorldServerIP;
	uint m_WorldServerPort;
	NetworkUUID m_SessionToken;
};