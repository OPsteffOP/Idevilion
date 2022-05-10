// Folder: Networking

#include "EnginePCH.h"
#include "NServer.h"

std::unordered_set<uint> NServer::m_BlacklistedIPs;

NServer::NServer()
	: m_ReplyCallback(nullptr)
{
	// TODO: load blacklisted IPs
}