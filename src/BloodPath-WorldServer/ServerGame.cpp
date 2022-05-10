#include "WorldServerPCH.h"
#include "ServerGame.h"

#include "ServerScene.h"

#include "NFactory.h"
#include "GamePackets.h"
#include "DatabasePackets.h"

ServerGame::~ServerGame()
{
	SAFE_DELETE(m_pServer);
	SAFE_DELETE(m_pScene);
}

void ServerGame::Initialize()
{
	if (!CommandLine::IsFlagSet("-server_port") || !CommandLine::IsFlagSet("-login_server") || !CommandLine::IsFlagSet("-login_port") || !CommandLine::IsFlagSet("-database_server") || !CommandLine::IsFlagSet("-database_port") ||
		!CommandLine::IsFlagSet("-world_file") || !CommandLine::IsFlagSet("-low_chunk_x") || !CommandLine::IsFlagSet("-low_chunk_y") || !CommandLine::IsFlagSet("-high_chunk_x") || !CommandLine::IsFlagSet("-high_chunk_y") ||
		!CommandLine::IsFlagSet("-login_auth_username") || !CommandLine::IsFlagSet("-login_auth_password") || !CommandLine::IsFlagSet("-database_auth_username") || !CommandLine::IsFlagSet("-database_auth_password"))
	{
		LOG_ERROR("Invalid world command line arguments, usage: -server_port=uint here -login_server=\"string here\" -login_port=uint here -database_server=\"string here\" -database_port=uint here"
			" -world_file=\"file location here (relative to data-server)\" -low_chunk_x=int here -low_chunk_y=int here -high_chunk_x=int here -high_chunk_y=int here"
			" -login_auth_username=\"username here\" -login_auth_password=\"password here\" -database_auth_username=\"username here\" -database_auth_password=\"username here\"");
		Shutdown();
		return;
	}

	if (!ConnectLoginServer())
	{
		Shutdown();
		return;
	}

	if (!ConnectDatabaseServer())
	{
		Shutdown();
		return;
	}

	//InitializeWorld();
	InitializeServer();
}

bool ServerGame::ConnectLoginServer()
{
	const std::string& loginHost = CommandLine::GetFlagValue("-login_server");
	const uint loginPort = (uint)std::stoi(CommandLine::GetFlagValue("-login_port"));

	m_pLoginServerClient = NFactory::CreateClient();
	m_pLoginServerClient->Connect(loginHost, loginPort);
	//m_pDatabaseClient->SetConnectionDiedCallback([]() { printf("Connection died detected\n"); }); // TODO: reconnect?

	if (!m_pLoginServerClient->IsConnected())
	{
		LOG_ERROR("Couldn't create a connection to the login server, server=%s:%d", loginHost.c_str(), loginPort);
		return false;
	}

	const std::string loginUsername = CommandLine::GetFlagValue("-login_auth_username");
	const std::string loginPassword = CommandLine::GetFlagValue("-login_auth_password");

	NPacket authPacket;
	authPacket.identifier = (NPacketIdentifier)GamePacket::EXPOSE_SERVER;
	authPacket.data.WriteByte((std::byte)loginUsername.length());
	authPacket.data.WriteString(loginUsername);
	authPacket.data.WriteByte((std::byte)loginPassword.length());
	authPacket.data.WriteString(loginPassword);

	m_pLoginServerClient->SendPacket(authPacket);
	NPacket receivedPacket = m_pLoginServerClient->ReadPacket();

	ServerAuthenticationStatus statusCode = (ServerAuthenticationStatus)receivedPacket.data.ReadUChar();
	if (statusCode != ServerAuthenticationStatus::SUCCESSFUL)
	{
		LOG_ERROR("Failed to authenticate to the login server, quitting as connection to the login server is necessary");
		m_pLoginServerClient->CloseConnection();
		return false;
	}

	return true;
}

bool ServerGame::ConnectDatabaseServer()
{
	const std::string& databaseHost = CommandLine::GetFlagValue("-database_server");
	const uint databasePort = (uint)std::stoi(CommandLine::GetFlagValue("-database_port"));

	m_pDatabaseClient = NFactory::CreateClient();
	m_pDatabaseClient->Connect(databaseHost, databasePort);
	//m_pDatabaseClient->SetConnectionDiedCallback([]() { printf("Connection died detected\n"); }); // TODO: reconnect?

	if (!m_pDatabaseClient->IsConnected())
	{
		LOG_ERROR("Couldn't create a connection to the database server, server=%s:%d", databaseHost.c_str(), databasePort);
		return false;
	}

	const std::string databaseUsername = CommandLine::GetFlagValue("-database_auth_username");
	const std::string databasePassword = CommandLine::GetFlagValue("-database_auth_password");

	NPacket authPacket;
	authPacket.identifier = (NPacketIdentifier)DatabasePacket::AUTHENTICATE_SERVER;
	authPacket.data.WriteByte((std::byte)DatabaseServerAuthenticationType::AUTHENTICATING_DATABASE);
	authPacket.data.WriteByte((std::byte)databaseUsername.length());
	authPacket.data.WriteString(databaseUsername);
	authPacket.data.WriteByte((std::byte)databasePassword.length());
	authPacket.data.WriteString(databasePassword);

	m_pDatabaseClient->SendPacket(authPacket);
	NPacket receivedPacket = m_pDatabaseClient->ReadPacket();

	DatabaseServerAuthenticationStatus statusCode = (DatabaseServerAuthenticationStatus)receivedPacket.data.ReadUChar();
	if (statusCode != DatabaseServerAuthenticationStatus::SUCCESSFUL)
	{
		LOG_ERROR("Failed to authenticate to the database server, quitting as connection to the database is necessary");
		m_pDatabaseClient->CloseConnection();
		return false;
	}

	return true;
}

void ServerGame::InitializeWorld()
{
	const std::string worldFileLocation = Paths::Data::DATA_SERVER_DIR + Paths::Data::WORLDS_DIR + CommandLine::GetFlagValue("-world_file");
	const int lowChunkX = std::stoi(CommandLine::GetFlagValue("-low_chunk_x"));
	const int lowChunkY = std::stoi(CommandLine::GetFlagValue("-low_chunk_y"));
	const int highChunkX = std::stoi(CommandLine::GetFlagValue("-high_chunk_x"));
	const int highChunkY = std::stoi(CommandLine::GetFlagValue("-high_chunk_y"));

	m_pScene = new ServerScene(worldFileLocation, Point2i(lowChunkX, lowChunkY), Point2i(highChunkX, highChunkY));
}

void ServerGame::InitializeServer()
{
	const uint serverPort = (uint)std::stoul(CommandLine::GetFlagValue("-server_port"));

	m_pServer = NFactory::CreateServer();
	m_pServer->SetPacketValidator([this](const NPacket& incomingPacket, uint& outPacketSize) { return Server_IncomingPacketValidator(incomingPacket, outPacketSize); });
	m_pServer->SetReplyCallback([this](NPacket& inOutPacket) { Server_PacketReplyCallback(inOutPacket); });
	m_pServer->StartListening(serverPort);
}

void ServerGame::Update()
{
	//m_pScene->Update();
}

bool ServerGame::Server_IncomingPacketValidator(const NPacket& incomingPacket, uint& outPacketSize)
{
	GamePacket identifier = (GamePacket)incomingPacket.identifier;
	switch (identifier)
	{
	case GamePacket::AUTHENTICATE:
	{
		const uint packetSize = (uint)incomingPacket.data.GetWriteBufferPointer();

		if (packetSize != sizeof(NetworkUUID))
		{
			LOG_ERROR("Incoming authentication packet is too small to contain a NetworkUUID");
			return false;
		}

		outPacketSize = sizeof(NetworkUUID);
		return true;
	}
	}

	LOG_ERROR("Incoming packet has an unknown packet identifier, dropping packet");
	return false;
}

void ServerGame::Server_PacketReplyCallback(NPacket& inOutPacket)
{
	GamePacket identifier = (GamePacket)inOutPacket.identifier;
	switch (identifier)
	{
	case GamePacket::AUTHENTICATE:
	{
		const NetworkUUID uuid = inOutPacket.data.ReadNetworkUUID();

		// TODO: REMOVE THIS
		printf("Authenticating: %s\n", (std::to_string(uuid.ipAddress) + "-" + std::to_string(uuid.timestamp) + "-" + std::to_string(uuid.random)).c_str());
		// TODO: REMOVE THIS

		// TODO
		//if (uuidHasNoAccessToThisWorldServer (e.g. members only))
		//{
		//	const VerifyStatus status = VerifyStatus::NO_ACCESS;

		//	inOutPacket.size = sizeof(uint8_t);
		//	std::copy((std::byte*)&status, ((std::byte*)&status) + sizeof(uint8_t), inOutPacket.networkPacket.data);

		//	break;
		//}

		NPacket verifyPacket;
		verifyPacket.identifier = (NPacketIdentifier)GamePacket::VERIFY;
		verifyPacket.data.WriteNetworkUUID(uuid);
		verifyPacket.data.WriteUInt32(inOutPacket.address);

		m_pLoginServerClient->SendPacket(verifyPacket);
		NPacket receivedPacket = m_pLoginServerClient->ReadPacket(&inOutPacket);
		LoginClient_HandleIncomingPacket(receivedPacket);

		break;
	}
	}
}

void ServerGame::LoginClient_HandleIncomingPacket(const NPacket& incomingPacket)
{
	GamePacket identifier = (GamePacket)incomingPacket.identifier;
	switch (identifier)
	{
	case GamePacket::VERIFY:
	{
		NPacket& authPacket = *reinterpret_cast<NPacket*>(incomingPacket.pUserData);
		authPacket.Reset();

		authPacket.identifier = (NPacketIdentifier)GamePacket::AUTHENTICATE;

		const UserVerifyStatus status = (UserVerifyStatus)incomingPacket.data.ReadUChar();
		authPacket.data.WriteUInt8((uint8_t)status);

		if (status == UserVerifyStatus::SUCCESSFUL)
		{
			const uint8_t privilegeLevel = incomingPacket.data.ReadUInt8();
			authPacket.data.WriteUInt8(privilegeLevel);
		}

		break;
	}
	}
}