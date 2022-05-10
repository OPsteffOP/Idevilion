// Folder: Databases

#pragma once
#include "FileDatabase.h"
#include "SecuritySettings.h"

class ServerDatabase
{
public:
	explicit ServerDatabase(const std::string& databasePath);

	bool ValidateCredentialsByUsername(const std::string& username, const std::string& password);

private:
	typedef FileDatabaseIndex<FileDatabaseStaticString<SecuritySettings::MAX_USERNAME_LENGTH>> UsernameIndex;

	FileDatabase<UsernameIndex> m_Database;
};