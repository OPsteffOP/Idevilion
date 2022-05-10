// Folder: Utils

#pragma once

PACK(1, struct NetworkUUID
{
	NetworkUUID() = default;
	explicit NetworkUUID(uint32_t ipAddress);

	uint32_t ipAddress;
	uint64_t timestamp;
	uint32_t random;

	bool IsValid() const;

	constexpr bool operator==(const NetworkUUID& rhs) const;
	constexpr bool operator!=(const NetworkUUID& rhs) const;

	struct Hasher
	{
		size_t operator()(const NetworkUUID& uuid) const;
	};

	static const NetworkUUID INVALID;
});