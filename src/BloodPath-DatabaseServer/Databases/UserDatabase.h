// Folder: Databases

#pragma once
#include "FileDatabase.h"
#include "SecuritySettings.h"

class UserDatabase
{
public:
	struct UserData
	{
		FileDatabaseStaticString<SecuritySettings::MAX_USERNAME_LENGTH> username;
		FileDatabaseStaticString<SecuritySettings::MAX_EMAIL_LENGTH> email;
		uint8_t privilegeStatus;
		int64_t banExpiration;
	};

public:
	explicit UserDatabase(const std::string& databasePath);

	bool ValidateCredentialsByUsername(const std::string& username, const std::string& password, UserData& userData);
	bool ValidateCredentialsByEmail(const std::string& email, const std::string& password, UserData& userData);

private:
	typedef FileDatabaseIndex<FileDatabaseStaticString<SecuritySettings::MAX_USERNAME_LENGTH>> UsernameIndex;
	typedef FileDatabaseIndex<FileDatabaseStaticString<SecuritySettings::MAX_EMAIL_LENGTH>> EmailIndex;

	FileDatabase<UsernameIndex, EmailIndex> m_Database;
};