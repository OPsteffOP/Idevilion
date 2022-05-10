// Folder: Databases

#include "DatabaseServerPCH.h"
#include "UserDatabase.h"

UserDatabase::UserDatabase(const std::string& databasePath)
	: m_Database(databasePath, UsernameIndex("username"), EmailIndex("email"))
{}

bool UserDatabase::ValidateCredentialsByUsername(const std::string& username, const std::string& password, UserData& userData)
{
	FileDatabaseStaticString<SecuritySettings::MAX_USERNAME_LENGTH> usernameStaticStr = username;

	PACK(1, struct DbData
	{
		FileDatabaseStaticString<SecuritySettings::MAX_USERNAME_LENGTH> username;
		FileDatabaseStaticString<SecuritySettings::MAX_EMAIL_LENGTH> email;
		FileDatabaseStaticString<SecuritySettings::MAX_PASSWORD_LENGTH> password;
		uint8_t privilegeStatus;
		int64_t banExpiration;
	});

	DbData data;
	if (!m_Database.GetDataValues("username", usernameStaticStr, { "username", "email", "password", "privilege_status", "ban_expiration" }, data))
		return false;

	if (data.password != password)
		return false;

	userData.username = data.username;
	userData.email = data.email;
	userData.privilegeStatus = data.privilegeStatus;
	userData.banExpiration = data.banExpiration;

	return true;
}

bool UserDatabase::ValidateCredentialsByEmail(const std::string& email, const std::string& password, UserData& userData)
{
	FileDatabaseStaticString<SecuritySettings::MAX_EMAIL_LENGTH> emailStaticStr = email;

	struct DbData
	{
		FileDatabaseStaticString<SecuritySettings::MAX_USERNAME_LENGTH> username;
		FileDatabaseStaticString<SecuritySettings::MAX_EMAIL_LENGTH> email;
		FileDatabaseStaticString<SecuritySettings::MAX_PASSWORD_LENGTH> password;
		uint8_t privilegeStatus;
		int64_t banExpiration;
	};

	DbData data;
	if (!m_Database.GetDataValues("email", emailStaticStr, { "username", "email", "password", "privilege_status", "ban_expiration" }, data))
		return false;

	if (data.password != password)
		return false;

	userData.username = data.username;
	userData.email = data.email;
	userData.privilegeStatus = data.privilegeStatus;
	userData.banExpiration = data.banExpiration;

	return true;
}