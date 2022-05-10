// Folder: ClientStates

#pragma once
#include "Game.h"

#include "NFactory.h"
#include "GamePackets.h"

class NClient;
class StreamingScene;

class ClientGame : public Game
{
public:
	ClientGame(const std::string& worldServerIP, uint worldServerPort, const NetworkUUID& sessionToken);
	virtual ~ClientGame() override;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Render() override;

protected:
	UserVerifyStatus ConnectWorldServer(const std::string& ip, uint port);

protected:
	constexpr static const char* WORLD_FILE_NAME = "lol\\lol";

	NetworkUUID m_SessionToken;
	NClient* m_pWorldServerClient;

	StreamingScene* m_pScene;
};