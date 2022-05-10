// Folder: ClientStates

#include "BloodPathPCH.h"
#include "LoginScreen.h"

void LoginScreen::Initialize()
{

}

void LoginScreen::Update()
{
	// TODO: IF LOGIN BUTTON PRESSED
	static bool temp = true;
	if (temp)
	{
		NClient* pLoginClient = NFactory::CreateClient();

		pLoginClient->SetConnectionDiedCallback([this, &pLoginClient]() { ServerConnectionDied(pLoginClient); });
		pLoginClient->Connect(LOGIN_SERVER_HOST, LOGIN_SERVER_PORT);

		if (!pLoginClient->IsConnected())
		{
			ServerConnectionDied(pLoginClient);
			return;
		}

		UserAuthenticationStatus status = Login(pLoginClient, "dev", "dev");
		if (status == UserAuthenticationStatus::SUCCESSFUL && m_SessionToken != NetworkUUID::INVALID)
		{
			// Login successful, transition to the next client state
			Shutdown();
		}
		else
		{
			// TODO: tell user what went wrong (ignore AuthenticationStatus::UNKNOWN - should be handled by ServerConnectionDied)
			LOG_DEBUG("Failed authentication with status code: %d", (uint)status);
		}

		SAFE_DELETE(pLoginClient);

		// TODO: remove this
		temp = false;
		// TODO: remove this
	}
}

void LoginScreen::Render()
{

}

void LoginScreen::ServerConnectionDied(NClient*& pClient)
{
	// TODO: tell user that the connection failed
	SAFE_DELETE(pClient);
}

UserAuthenticationStatus LoginScreen::Login(NClient* pClient, const std::string& login, const std::string& password)
{
	NPacket authPacket;
	authPacket.identifier = (NPacketIdentifier)GamePacket::AUTHENTICATE;
	authPacket.data.WriteUInt16(CLIENT_VERSION);
	authPacket.data.WriteByte((std::byte)login.size());
	authPacket.data.WriteString(login);
	authPacket.data.WriteByte((std::byte)password.size());
	authPacket.data.WriteString(password);

	pClient->SendPacket(authPacket);

	NPacket receivedPacket = pClient->ReadPacket();
	if ((GamePacket)receivedPacket.identifier != GamePacket::AUTHENTICATE)
		return UserAuthenticationStatus::UNKNOWN;

	UserAuthenticationStatus status = (UserAuthenticationStatus)receivedPacket.data.ReadUChar();

	if (status == UserAuthenticationStatus::SUCCESSFUL)
	{
		uint32_t worldServerAddress = receivedPacket.data.ReadUInt32();
		uint16_t worldServerPort = receivedPacket.data.ReadUInt16();
		m_SessionToken = receivedPacket.data.ReadNetworkUUID();

		char readableAddressBuffer[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &worldServerAddress, readableAddressBuffer, sizeof(readableAddressBuffer)) == NULL)
		{
			LOG_ERROR("Failed to convert world server address to readable address, error_code=%d", WSAGetLastError());
			return UserAuthenticationStatus::UNKNOWN;
		}

		m_WorldServerIP = readableAddressBuffer;
		m_WorldServerPort = worldServerPort;
	}

	return status;
}