#include "LoginServerPCH.h"
#include "LoginServer.h"

#include "GamePackets.h"
#include "DatabasePackets.h"
#include "SecuritySettings.h"

LoginServer::LoginServer()
	: m_pServer(nullptr)
	, m_pDatabaseClient(nullptr)
{
	m_pServer = NFactory::CreateServer();
	m_pServer->SetPacketValidator(Server_IncomingPacketValidator);
	m_pServer->SetReplyCallback([this](NPacket& inOutPacket) { Server_PacketReplyCallback(inOutPacket); });
}

LoginServer::~LoginServer()
{
	SAFE_DELETE(m_pServer);
	SAFE_DELETE(m_pDatabaseClient);
}

bool LoginServer::ConnectDatabase(const std::string& host, uint port, const std::string& username, const std::string& password)
{
	m_pDatabaseClient = NFactory::CreateClient();
	m_pDatabaseClient->Connect(host, port);
	//m_pDatabaseClient->SetConnectionDiedCallback([]() { printf("Connection died detected\n"); }); // TODO: reconnect?

	if (!m_pDatabaseClient->IsConnected())
	{
		LOG_ERROR("Couldn't create a connection to the database server, server=%s:%d", host.c_str(), port);
		return false;
	}

	NPacket authPacket;
	authPacket.identifier = (NPacketIdentifier)DatabasePacket::AUTHENTICATE_SERVER;
	authPacket.data.WriteByte((std::byte)DatabaseServerAuthenticationType::AUTHENTICATING_DATABASE);
	authPacket.data.WriteByte((std::byte)username.length());
	authPacket.data.WriteString(username);
	authPacket.data.WriteByte((std::byte)password.length());
	authPacket.data.WriteString(password);

	m_pDatabaseClient->SendPacket(authPacket);
	NPacket receivedPacket = m_pDatabaseClient->ReadPacket();
	DatabaseClient_HandleIncomingPacket(receivedPacket);

	return m_pDatabaseClient->IsConnected();
}

// 
// UNCOMMENT THIS Start FUNCTION TO CHECK MEMORY LEAKS
// This will just create a window that you can close to safely shutdown the server
// 
//#include "WindowManager.h"
//void LoginServer::Start(uint port)
//{
//	m_pServer->StartListening(port, true);
//
//	WindowManager::GetInstance()->CreateWindow(WindowIdentifier::MAIN_WINDOW, "Login Server", 720, 480);
//	while (!WindowManager::GetInstance()->GetWindow(WindowIdentifier::MAIN_WINDOW)->IsClosed())
//	{
//		WindowManager::GetInstance()->GetWindow(WindowIdentifier::MAIN_WINDOW)->Update();
//	}
//}

void LoginServer::Start(uint port)
{
	m_pServer->StartListening(port, false);
}

bool LoginServer::Server_IncomingPacketValidator(const NPacket& incomingPacket, uint& outPacketSize)
{
	GamePacket identifier = (GamePacket)incomingPacket.identifier;
	switch (identifier)
	{
	case GamePacket::EXPOSE_SERVER:
	{
		const uint packetSize = (uint)incomingPacket.data.GetWriteBufferPointer();

		if (packetSize < sizeof(std::byte) + 1 + sizeof(std::byte) + 1)
		{
			LOG_ERROR("Incoming expose server packet is too small to contain the minimum amount of data");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer(0);
		std::byte usernameLength = incomingPacket.data.ReadByte();
		if (packetSize < sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + 1)
		{
			LOG_ERROR("Incoming expose server packet is too small to contain the username and a 1 byte password");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer((uint)(sizeof(std::byte) + (uint)usernameLength));
		std::byte passwordLength = incomingPacket.data.ReadByte();
		if (packetSize < sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + (uint)passwordLength)
		{
			LOG_ERROR("Incoming expose server packet is too small to contain the authentication type, username and password");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer(0);
		outPacketSize = sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + (uint)passwordLength;
		return true;
	}
	case GamePacket::AUTHENTICATE:
	{
		const uint packetSize = (uint)incomingPacket.data.GetWriteBufferPointer();

		if (packetSize < sizeof(int16_t) + sizeof(std::byte) + 1 + sizeof(std::byte) + 1)
		{
			LOG_ERROR("Incoming authentication packet is too small to contain the minimum amount of data");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer((uint)(sizeof(int16_t)));
		std::byte usernameLength = incomingPacket.data.ReadByte();
		if (packetSize < sizeof(int16_t) + sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + 1)
		{
			LOG_ERROR("Incoming authentication packet is too small to contain the username and a 1 byte password");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer((uint)(sizeof(int16_t) + sizeof(std::byte) + (uint)usernameLength));
		std::byte passwordLength = incomingPacket.data.ReadByte();
		if (packetSize < sizeof(int16_t) + sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + (uint)passwordLength)
		{
			LOG_ERROR("Incoming authentication packet is too small to contain the username and password");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer(0);
		outPacketSize = sizeof(int16_t) + sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + (uint)passwordLength;
		return true;
	}
	case GamePacket::VERIFY:
	{
		const uint packetSize = (uint)incomingPacket.data.GetWriteBufferPointer();

		if (packetSize < sizeof(uint32_t) + sizeof(NetworkUUID))
		{
			LOG_ERROR("Incoming verification packet is too small to contain the minimum amount of data");
			return false;
		}

		outPacketSize = sizeof(uint32_t) + sizeof(NetworkUUID);
		return true;
	}
	}

	LOG_ERROR("Incoming packet has an unknown packet identifier, dropping packet");
	return false;
}

void LoginServer::Server_PacketReplyCallback(NPacket& inOutPacket)
{
	GamePacket identifier = (GamePacket)inOutPacket.identifier;
	switch (identifier)
	{
	case GamePacket::EXPOSE_SERVER:
	{
		const uint usernameLength = (uint)inOutPacket.data.ReadByte();
		const std::string username = inOutPacket.data.ReadString(usernameLength);
		const uint passwordLength = (uint)inOutPacket.data.ReadByte();
		const std::string password = inOutPacket.data.ReadString(passwordLength);

		printf("Authenticating (server): %s:%s\n", username.c_str(), password.c_str());

		NPacket authPacket;
		authPacket.identifier = (NPacketIdentifier)DatabasePacket::AUTHENTICATE_SERVER;
		authPacket.data.WriteByte((std::byte)DatabaseServerAuthenticationType::CHECKING_CREDENTIALS);
		authPacket.data.WriteByte((std::byte)username.length());
		authPacket.data.WriteString(username);
		authPacket.data.WriteByte((std::byte)password.length());
		authPacket.data.WriteString(password);

		m_pDatabaseClient->SendPacket(authPacket);
		NPacket receivedPacket = m_pDatabaseClient->ReadPacket(&inOutPacket);
		DatabaseClient_HandleIncomingPacket(receivedPacket);

		break;
	}
	case GamePacket::AUTHENTICATE:
	{
		const uint16_t clientVersion = inOutPacket.data.ReadUInt16();

		const uint usernameLength = (uint)inOutPacket.data.ReadByte();
		const std::string username = inOutPacket.data.ReadString(usernameLength);
		const uint passwordLength = (uint)inOutPacket.data.ReadByte();
		const std::string password = inOutPacket.data.ReadString(passwordLength);

		const DatabaseUserAuthenticationType authType = (username.find('@') == std::string::npos) ? DatabaseUserAuthenticationType::AUTH_USERNAME : DatabaseUserAuthenticationType::AUTH_EMAIL;

		// TODO: REMOVE THIS
		printf("Authenticating (user): %s:%s (version: %d)\n", username.c_str(), password.c_str(), clientVersion);
		// TODO: REMOVE THIS

		if (clientVersion != 1) // TODO: actual client version
		{
			inOutPacket.Reset();
			inOutPacket.identifier = (NPacketIdentifier)GamePacket::AUTHENTICATE;
			inOutPacket.data.WriteByte((std::byte)UserAuthenticationStatus::OUT_OF_DATE_CLIENT);

			break;
		}

		NPacket authPacket;
		authPacket.identifier = (NPacketIdentifier)DatabasePacket::AUTHENTICATE_USER;
		authPacket.data.WriteByte((std::byte)authType);
		authPacket.data.WriteByte((std::byte)username.length());
		authPacket.data.WriteString(username);
		authPacket.data.WriteByte((std::byte)password.length());
		authPacket.data.WriteString(password);

		m_pDatabaseClient->SendPacket(authPacket);
		NPacket receivedPacket = m_pDatabaseClient->ReadPacket(&inOutPacket);
		DatabaseClient_HandleIncomingPacket(receivedPacket);

		break;
	}
	case GamePacket::VERIFY:
	{
		const NetworkUUID uuid = inOutPacket.data.ReadNetworkUUID();
		const uint32_t userAddress = inOutPacket.data.ReadUInt32();

		// TODO: REMOVE THIS
		printf("Verifying: %s\n", (std::to_string(uuid.ipAddress) + "-" + std::to_string(uuid.timestamp) + "-" + std::to_string(uuid.random)).c_str());
		// TODO: REMOVE THIS

		UserVerifyStatus status = UserVerifyStatus::FAILURE;
		{
			std::unique_lock lock(m_SessionTokensMutex);
			if (uuid.ipAddress == userAddress && m_SessionTokens.find(uuid) != m_SessionTokens.end()
				&& std::chrono::duration_cast<std::chrono::duration<uint64_t, std::milli>>(std::chrono::system_clock::now().time_since_epoch()).count() - uuid.timestamp < SESSION_TOKEN_EXPIRATION_MILLISECONDS)
			{
				status = UserVerifyStatus::SUCCESSFUL;
			}
		}

		inOutPacket.Reset();
		inOutPacket.identifier = (NPacketIdentifier)GamePacket::VERIFY;

		inOutPacket.data.WriteByte((std::byte)status);

		if (status == UserVerifyStatus::SUCCESSFUL)
		{
			const uint8_t privilegeLevel = 0; // TODO
			inOutPacket.data.WriteUInt8(privilegeLevel);
		}

		break;
	}
	}
}

void LoginServer::DatabaseClient_HandleIncomingPacket(const NPacket& incomingPacket)
{
	DatabasePacket identifier = (DatabasePacket)incomingPacket.identifier;
	switch (identifier)
	{
	case DatabasePacket::AUTHENTICATE_SERVER:
	{
		DatabaseServerAuthenticationStatus statusCode = (DatabaseServerAuthenticationStatus)incomingPacket.data.ReadUChar();

		if (incomingPacket.pUserData == nullptr)
		{
			// Authenticating login server to the database
			if (statusCode != DatabaseServerAuthenticationStatus::SUCCESSFUL)
			{
				LOG_ERROR("Failed to authenticate to the database server, quitting as connection to the database is necessary");
				m_pDatabaseClient->CloseConnection();
			}
		}
		else
		{
			// Checking credentials in the database to authenticate a world server to the login server
			NPacket& exposePacket = *reinterpret_cast<NPacket*>(incomingPacket.pUserData);
			exposePacket.Reset();

			ServerAuthenticationStatus status = ServerAuthenticationStatus::FAILURE;
			if (statusCode == DatabaseServerAuthenticationStatus::SUCCESSFUL)
				status = ServerAuthenticationStatus::SUCCESSFUL;

			exposePacket.identifier = (NPacketIdentifier)GamePacket::EXPOSE_SERVER;
			exposePacket.data.WriteByte((std::byte)status);

			if (statusCode == DatabaseServerAuthenticationStatus::SUCCESSFUL)
			{
				// TODO: add world server as exposed server
			}
		}

		break;
	}
	case DatabasePacket::AUTHENTICATE_USER:
	{
		NPacket& loginPacket = *reinterpret_cast<NPacket*>(incomingPacket.pUserData);

		DatabaseUserAuthenticationStatus statusCode = (DatabaseUserAuthenticationStatus)incomingPacket.data.ReadUChar();
		if (statusCode == DatabaseUserAuthenticationStatus::INCORRECT_CREDENTIALS)
		{
			loginPacket.Reset();
			loginPacket.identifier = (NPacketIdentifier)GamePacket::AUTHENTICATE;
			loginPacket.data.WriteByte((std::byte)UserAuthenticationStatus::INCORRECT_CREDENTIALS);
		}
		else if (statusCode == DatabaseUserAuthenticationStatus::BANNED)
		{
			loginPacket.Reset();
			loginPacket.identifier = (NPacketIdentifier)GamePacket::AUTHENTICATE;
			loginPacket.data.WriteByte((std::byte)UserAuthenticationStatus::BANNED);
		}
		else
		{
			const uint32_t worldServerAddress = 16777343; // TODO
			const uint16_t worldServerPort = 12345; // TODO

			loginPacket.data.SetReadBufferPointer((uint)(sizeof(uint16_t)));
			const uint usernameLength = (uint)loginPacket.data.ReadByte();
			const std::string username = loginPacket.data.ReadString(usernameLength);

			NetworkUUID uuid(loginPacket.address);
			{
				std::unique_lock lock(m_SessionTokensMutex);

				while (m_SessionTokens.find(uuid) != m_SessionTokens.end())
				{
					uuid = NetworkUUID(loginPacket.address);
				}

				m_SessionTokens.emplace(uuid, username);
			}

			loginPacket.Reset();
			loginPacket.identifier = (NPacketIdentifier)GamePacket::AUTHENTICATE;
			loginPacket.data.WriteByte((std::byte)UserAuthenticationStatus::SUCCESSFUL);
			loginPacket.data.WriteUInt32(worldServerAddress);
			loginPacket.data.WriteUInt16(worldServerPort);
			loginPacket.data.WriteNetworkUUID(uuid);
		}

		break;
	}
	}
}