// Folder: Networking

#pragma once
#include "NPacket.h"

class NServer
{
public:
	typedef std::function<void(const NetworkUUID& uuid)> ClientDroppedCallback;
	typedef std::function<bool(const NPacket& incomingPacket, uint& outPacketSize)> IncomingPacketValidator;
	typedef std::function<void(NPacket& inOutPacket)> ConstructReplyPacketCallback;

public:
	NServer();
	virtual ~NServer() = default;

	virtual void StartListening(uint port, bool shouldUseAcceptThread = true) = 0;

	virtual bool IsClientConnected(const NetworkUUID& uuid) const = 0;
	virtual void CloseClient(const NetworkUUID& uuid) = 0;

	void SetClientDroppedCallback(ClientDroppedCallback callback) { m_ClientDroppedCallback = callback; }
	void SetPacketValidator(IncomingPacketValidator callback) { m_PacketValidator = callback; }
	void SetReplyCallback(ConstructReplyPacketCallback callback) { m_ReplyCallback = callback; }

protected:
	static std::unordered_set<uint> m_BlacklistedIPs;

	ClientDroppedCallback m_ClientDroppedCallback;
	IncomingPacketValidator m_PacketValidator;
	ConstructReplyPacketCallback m_ReplyCallback;
};