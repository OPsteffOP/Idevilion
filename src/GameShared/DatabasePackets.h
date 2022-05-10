enum class DatabasePacket : NPacketIdentifier
{
	INVALID,

	AUTHENTICATE_SERVER,
	AUTHENTICATE_USER,
};

DEFINE_ENUM(DatabaseServerAuthenticationType : unsigned char,
	AUTHENTICATING_DATABASE,
	CHECKING_CREDENTIALS
);

DEFINE_ENUM(DatabaseServerAuthenticationStatus : unsigned char,
	SUCCESSFUL,
	FAILURE
);

DEFINE_ENUM(DatabaseUserAuthenticationType : unsigned char,
	AUTH_USERNAME,
	AUTH_EMAIL
);

DEFINE_ENUM(DatabaseUserAuthenticationStatus : unsigned char,
	SUCCESSFUL,
	INCORRECT_CREDENTIALS,
	BANNED
);