// Folder: Networking

#pragma once
#include "ByteStream.h"

typedef uint8_t NPacketIdentifier;

#define INVALID_PACKET_IDENTIFIER 0

struct NPacket
{
	NPacket();

	NPacket(const NPacket& packet) = delete;
	NPacket(NPacket&& packet) noexcept;
	NPacket& operator=(const NPacket& packet) = delete;
	NPacket& operator=(NPacket&& packet) noexcept;

	void Reset();
	void ConstructPacket(char* pBuffer, uint length);

	constexpr static const uint MAX_HEADER_SIZE = sizeof(NPacketIdentifier);
	constexpr static const uint MAX_BODY_SIZE = 1024;
	constexpr static const uint MAX_PACKET_SIZE = MAX_HEADER_SIZE + MAX_BODY_SIZE;

	void* pUserData;

	// Server data
	NetworkUUID clientUUID;
	uint32_t address;
	std::string readableAddress;

	// Network data
	NPacketIdentifier identifier;
	ByteStream data;

	// NPacket will only get sent over the network
	// The other stuff in NPacketExt is just for ease of use
	//PACK(1, struct NPacket
	//{
	//	constexpr static const uint MAX_HEADER_SIZE = sizeof(NPacketIdentifier);
	//	constexpr static const uint MAX_BODY_SIZE = 1024;
	//	constexpr static const uint MAX_PACKET_SIZE = MAX_HEADER_SIZE + MAX_BODY_SIZE;

	//	// Header
	//	NPacketIdentifier identifier;

	//	// Body
	//	std::byte data[MAX_BODY_SIZE];
	//});
	//NPacket networkPacket;
};