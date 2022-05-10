#include "DatabaseServerPCH.h"
#include "DatabaseServer.h"

#include "NFactory.h"
#include "DatabasePackets.h"
#include "SecuritySettings.h"
#include "Databases.h"

DatabaseServer::DatabaseServer()
	: m_pServer(nullptr)
{
	m_pServer = NFactory::CreateServer();
	m_pServer->SetClientDroppedCallback([this](const NetworkUUID& uuid) { Server_ClientDroppedCallback(uuid); });
	m_pServer->SetPacketValidator([this](const NPacket& incomingPacket, uint& outPacketSize) { return Server_IncomingPacketValidator(incomingPacket, outPacketSize); });
	m_pServer->SetReplyCallback([this](NPacket& inOutPacket) { Server_PacketReplyCallback(inOutPacket); });
}

DatabaseServer::~DatabaseServer()
{
	SAFE_DELETE(m_pServer);
}

// 
// UNCOMMENT THIS Start FUNCTION TO CHECK MEMORY LEAKS
// This will just create a window that you can close to safely shutdown the server
// 
//#include "WindowManager.h"
//void DatabaseServer::Start(uint port)
//{
//	m_pServer->StartListening(port, true);
//
//	WindowManager::GetInstance()->CreateWindow(WindowIdentifier::MAIN_WINDOW, "Database Server", 720, 480);
//	while (!WindowManager::GetInstance()->GetWindow(WindowIdentifier::MAIN_WINDOW)->IsClosed())
//	{
//		WindowManager::GetInstance()->GetWindow(WindowIdentifier::MAIN_WINDOW)->Update();
//	}
//}

void DatabaseServer::Start(uint port)
{
	m_pServer->StartListening(port, false);
}

void DatabaseServer::Server_ClientDroppedCallback(const NetworkUUID& uuid)
{
	std::unique_lock lock(m_SessionTokensMutex);
	m_SessionTokens.erase(uuid);
}

bool DatabaseServer::Server_IncomingPacketValidator(const NPacket& incomingPacket, uint& outPacketSize)
{
	DatabasePacket identifier = (DatabasePacket)incomingPacket.identifier;
	switch (identifier)
	{
	case DatabasePacket::AUTHENTICATE_SERVER:
	{
		const uint packetSize = (uint)incomingPacket.data.GetWriteBufferPointer();

		if (packetSize < sizeof(std::byte) + sizeof(std::byte) + 1 + sizeof(std::byte) + 1)
		{
			LOG_ERROR("Incoming server authentication packet is too small to contain the minimum amount of data");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer((uint)(sizeof(std::byte)));
		std::byte usernameLength = incomingPacket.data.ReadByte();
		if (packetSize < sizeof(std::byte) + sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + 1)
		{
			LOG_ERROR("Incoming server authentication packet is too small to contain the authentication type, username and a 1 byte password");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer((uint)(sizeof(std::byte) + sizeof(std::byte) + (uint)usernameLength));
		std::byte passwordLength = incomingPacket.data.ReadByte();
		if (packetSize < sizeof(std::byte) + sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + (uint)passwordLength)
		{
			LOG_ERROR("Incoming server authentication packet is too small to contain the authentication type, username and password");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer(0);
		outPacketSize = sizeof(std::byte) + sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + (uint)passwordLength;
		return true;
	}
	case DatabasePacket::AUTHENTICATE_USER:
	{
		{
			std::unique_lock lock(m_SessionTokensMutex);
			if (m_SessionTokens.find(incomingPacket.clientUUID) == m_SessionTokens.end())
			{
				lock.unlock();
				m_pServer->CloseClient(incomingPacket.clientUUID);

				LOG_ERROR("Requesting client isn't authenticated");
				return false;
			}
		}

		const uint packetSize = (uint)incomingPacket.data.GetWriteBufferPointer();

		if (packetSize < sizeof(uint8_t) + sizeof(std::byte) + 1 + sizeof(std::byte) + 1)
		{
			LOG_ERROR("Incoming user authentication packet is too small to contain the minimum amount of data");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer(0);
		DatabaseUserAuthenticationType authType = (DatabaseUserAuthenticationType)incomingPacket.data.ReadUChar();
		if (authType >= DatabaseUserAuthenticationType::_COUNT)
		{
			LOG_ERROR("Incoming user authentication packet contains an unknown authentication type");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer((uint)(sizeof(uint8_t)));
		std::byte usernameLength = incomingPacket.data.ReadByte();
		if (packetSize < sizeof(uint8_t) + sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + 1)
		{
			LOG_ERROR("Incoming user authentication packet is too small to contain the username and a 1 byte password");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer((uint)(sizeof(uint8_t) + sizeof(std::byte) + (uint)usernameLength));
		std::byte passwordLength = incomingPacket.data.ReadByte();
		if (packetSize < sizeof(uint8_t) + sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + (uint)passwordLength)
		{
			LOG_ERROR("Incoming user authentication packet is too small to contain the username and password");
			return false;
		}

		incomingPacket.data.SetReadBufferPointer(0);
		outPacketSize = sizeof(uint8_t) + sizeof(std::byte) + (uint)usernameLength + sizeof(std::byte) + (uint)passwordLength;
		return true;
	}
	}

	return false;
}

void DatabaseServer::Server_PacketReplyCallback(NPacket& inOutPacket)
{
	DatabasePacket identifier = (DatabasePacket)inOutPacket.identifier;
	switch (identifier)
	{
	case DatabasePacket::AUTHENTICATE_SERVER:
	{
		const DatabaseServerAuthenticationType authType = (DatabaseServerAuthenticationType)inOutPacket.data.ReadUChar();
		const uint usernameLength = (uint)inOutPacket.data.ReadByte();
		const std::string username = inOutPacket.data.ReadString(usernameLength);
		const uint passwordLength = (uint)inOutPacket.data.ReadByte();
		const std::string password = inOutPacket.data.ReadString(passwordLength);

		printf("Authenticating (server): %s:%s (type: %d)\n", username.c_str(), password.c_str(), (uint)authType);

		DatabaseServerAuthenticationStatus status;
		if (Databases::pServerDatabase->ValidateCredentialsByUsername(username, password))
			status = DatabaseServerAuthenticationStatus::SUCCESSFUL;
		else
			status = DatabaseServerAuthenticationStatus::FAILURE;

		inOutPacket.Reset();
		inOutPacket.identifier = (NPacketIdentifier)DatabasePacket::AUTHENTICATE_SERVER;
		inOutPacket.data.WriteByte((std::byte)status);

		if (authType == DatabaseServerAuthenticationType::AUTHENTICATING_DATABASE && status == DatabaseServerAuthenticationStatus::SUCCESSFUL)
		{
			std::unique_lock lock(m_SessionTokensMutex);
			m_SessionTokens.emplace(inOutPacket.clientUUID);
		}

		break;
	}
	case DatabasePacket::AUTHENTICATE_USER:
	{
		DatabaseUserAuthenticationType authType = (DatabaseUserAuthenticationType)inOutPacket.data.ReadUChar();

		const uint loginLength = (uint)inOutPacket.data.ReadByte();
		const std::string login = inOutPacket.data.ReadString(loginLength);
		const uint passwordLength = (uint)inOutPacket.data.ReadByte();
		const std::string password = inOutPacket.data.ReadString(passwordLength);

		printf("Authenticating (user): %s:%s (type: %d)\n", login.c_str(), password.c_str(), authType);

		bool success = false;
		UserDatabase::UserData data;
		if (authType == DatabaseUserAuthenticationType::AUTH_USERNAME)
			success = Databases::pUserDatabase->ValidateCredentialsByUsername(login, password, data);
		else if (authType == DatabaseUserAuthenticationType::AUTH_EMAIL)
			success = Databases::pUserDatabase->ValidateCredentialsByEmail(login, password, data);

		DatabaseUserAuthenticationStatus status;
		// TODO: check ban expiration
		if (!success)
			status = DatabaseUserAuthenticationStatus::INCORRECT_CREDENTIALS;
		else
		{
			// TODO: check for ban
			status = DatabaseUserAuthenticationStatus::SUCCESSFUL;
		}

		inOutPacket.Reset();
		inOutPacket.identifier = (NPacketIdentifier)DatabasePacket::AUTHENTICATE_USER;
		inOutPacket.data.WriteByte((std::byte)status);

		break;
	}
	}
}