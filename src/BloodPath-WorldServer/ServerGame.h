#pragma once
#include "Game.h"
#include "NPacket.h"

class ServerScene;
class NServer;
class NClient;

class ServerGame : public Game
{
public:
	virtual ~ServerGame() override;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Render() override {}

protected:
	bool Server_IncomingPacketValidator(const NPacket& incomingPacket, uint& outPacketSize);
	void Server_PacketReplyCallback(NPacket& inOutPacket);

	void LoginClient_HandleIncomingPacket(const NPacket& incomingPacket);

	bool ConnectLoginServer();
	bool ConnectDatabaseServer();
	void InitializeWorld();
	void InitializeServer();

protected:
	ServerScene* m_pScene;
	NServer* m_pServer;
	NClient* m_pDatabaseClient;
	NClient* m_pLoginServerClient;
};