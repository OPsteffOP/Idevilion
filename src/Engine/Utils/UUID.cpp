// Folder: Utils

#include "EnginePCH.h"
#include "UUID.h"

const NetworkUUID NetworkUUID::INVALID = NetworkUUID();

NetworkUUID::NetworkUUID(uint32_t ipAddress)
	: ipAddress(ipAddress)
	, timestamp(std::chrono::duration_cast<std::chrono::duration<uint64_t, std::milli>>(std::chrono::system_clock::now().time_since_epoch()).count())
	, random(/* TODO */)
{}

bool NetworkUUID::IsValid() const
{
	return *this != INVALID;
}

constexpr bool NetworkUUID::operator==(const NetworkUUID& rhs) const
{
	return ipAddress == rhs.ipAddress &&
		timestamp == rhs.timestamp &&
		random == rhs.random;
}

constexpr bool NetworkUUID::operator!=(const NetworkUUID& rhs) const
{
	return !(*this == rhs);
}

size_t NetworkUUID::Hasher::operator()(const NetworkUUID& uuid) const
{
	size_t hash = 0;
	Utils::HashCombine(hash, uuid.ipAddress);
	Utils::HashCombine(hash, uuid.timestamp);
	Utils::HashCombine(hash, uuid.random);

	return hash;
}