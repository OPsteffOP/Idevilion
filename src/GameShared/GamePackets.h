#pragma once

enum class GamePacket : NPacketIdentifier
{
	INVALID,

	EXPOSE_SERVER,
	AUTHENTICATE,
	VERIFY,
};

DEFINE_ENUM(ServerAuthenticationStatus : unsigned char,
	SUCCESSFUL,
	FAILURE
);

DEFINE_ENUM(UserAuthenticationStatus : unsigned char,
	SUCCESSFUL,
	OUT_OF_DATE_CLIENT,
	INCORRECT_CREDENTIALS,
	BANNED,

	UNKNOWN
);

DEFINE_ENUM(UserVerifyStatus : unsigned char,
	SUCCESSFUL,
	FAILURE,
	NO_ACCESS,

	SERVER_DOWN
);