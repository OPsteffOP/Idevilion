// Folder: Databases

#include "DatabaseServerPCH.h"
#include "ServerDatabase.h"

ServerDatabase::ServerDatabase(const std::string& databasePath)
	: m_Database(databasePath, UsernameIndex("username"))
{}

bool ServerDatabase::ValidateCredentialsByUsername(const std::string& username, const std::string& password)
{
	FileDatabaseStaticString<SecuritySettings::MAX_USERNAME_LENGTH> usernameStaticStr = username;

	PACK(1, struct DbData
	{
		FileDatabaseStaticString<SecuritySettings::MAX_USERNAME_LENGTH> username;
		FileDatabaseStaticString<SecuritySettings::MAX_PASSWORD_LENGTH> password;
	});

	DbData data;
	if (!m_Database.GetDataValues("username", usernameStaticStr, { "username", "password" }, data))
		return false;

	if (data.password != password)
		return false;

	return true;
}