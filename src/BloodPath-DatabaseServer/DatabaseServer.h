#pragma once

#include "NFactory.h"

class DatabaseServer
{
public:
	DatabaseServer();
	~DatabaseServer();

	void Start(uint port);

private:
	void Server_ClientDroppedCallback(const NetworkUUID& uuid);
	bool Server_IncomingPacketValidator(const NPacket& incomingPacket, uint& outPacketSize);
	void Server_PacketReplyCallback(NPacket& inOutPacket);

private:
	NServer* m_pServer;

	std::mutex m_SessionTokensMutex;
	std::unordered_set<NetworkUUID, NetworkUUID::Hasher> m_SessionTokens;
};