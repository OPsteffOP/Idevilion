#pragma once

#include "NFactory.h"

class LoginServer
{
public:
	LoginServer();
	~LoginServer();

	bool ConnectDatabase(const std::string& host, uint port, const std::string& username, const std::string& password);
	void Start(uint port);

private:
	static bool Server_IncomingPacketValidator(const NPacket& incomingPacket, uint& outPacketSize);
	void Server_PacketReplyCallback(NPacket& inOutPacket);

	void DatabaseClient_HandleIncomingPacket(const NPacket& incomingPacket);

private:
	constexpr static const uint SESSION_TOKEN_EXPIRATION_MILLISECONDS = 12 * 60 * 60 * 1000; // 12 hours

	NServer* m_pServer;
	NClient* m_pDatabaseClient;

	std::mutex m_SessionTokensMutex;
	std::unordered_map<NetworkUUID, std::string, NetworkUUID::Hasher> m_SessionTokens;
};