// Folder: Networking

#include "EnginePCH.h"
#include "NPacket.h"

NPacket::NPacket()
	: data(new ByteStreamBufferImpl<MAX_BODY_SIZE>())
{}

/*
	void* pUserData;

	// Server data
	NetworkUUID clientUUID;
	uint32_t address;
	std::string readableAddress;

	// Network data
	NPacketIdentifier identifier;
	ByteStream data;
*/

NPacket::NPacket(NPacket&& packet) noexcept
	: pUserData(packet.pUserData)
	, clientUUID(std::move(packet.clientUUID))
	, address(packet.address)
	, readableAddress(std::move(packet.readableAddress))
	, identifier(packet.identifier)
	, data(std::move(packet.data))
{
	packet.pUserData = nullptr;
	packet.clientUUID = NetworkUUID::INVALID;
	packet.address = 0;
	packet.readableAddress = "";
	packet.identifier = 0;
}

NPacket& NPacket::operator=(NPacket&& packet) noexcept
{
	pUserData = packet.pUserData;
	clientUUID = std::move(packet.clientUUID);
	address = packet.address;
	readableAddress = std::move(packet.readableAddress);
	identifier = packet.identifier;
	data = std::move(packet.data);

	packet.pUserData = nullptr;
	packet.clientUUID = NetworkUUID::INVALID;
	packet.address = 0;
	packet.readableAddress = "";
	packet.identifier = 0;

	return *this;
}

void NPacket::Reset()
{
	identifier = INVALID_PACKET_IDENTIFIER;
	data.Reset();
}

void NPacket::ConstructPacket(char* pBuffer, uint length)
{
	if (length < NPacket::MAX_HEADER_SIZE)
	{
		LOG_ERROR("Received network packet is smaller than NPacket::MAX_HEADER_SIZE, invalid packet");
		identifier = INVALID_PACKET_IDENTIFIER;
		return;
	}

	//if (length == NPacket::MAX_HEADER_SIZE)
	//{
	//	LOG_ERROR("Received network packet is exactly the size of NPacket::MAX_HEADER_SIZE, no data attached, invalid packet");
	//	networkPacket.identifier = INVALID_PACKET_IDENTIFIER;
	//	return;
	//}

	if (length > NPacket::MAX_PACKET_SIZE)
	{
		LOG_ERROR("Received network packet is bigger than NPacket::MAX_PACKET_SIZE, invalid packet");
		identifier = INVALID_PACKET_IDENTIFIER;
		return;
	}

	identifier = (NPacketIdentifier)pBuffer[0];
	data.WriteData(pBuffer + NPacket::MAX_HEADER_SIZE, length - NPacket::MAX_HEADER_SIZE, Endian::BIG_ENDIAN);
}