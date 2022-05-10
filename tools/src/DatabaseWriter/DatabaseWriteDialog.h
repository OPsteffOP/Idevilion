#pragma once
#include "ByteStream.h"

class DatabaseWriteDialog
{
public:
	void Start();

private:
	void RequestDatabasePath();
	int RequestDatabaseOperation();

	static void DatabaseOperation_AddColumn(DatabaseWriteDialog* pInstance);
	static void DatabaseOperation_AddRow(DatabaseWriteDialog* pInstance);

	static void WriteIntegralData(uint columnSize, const std::string& dataStr, ByteStream& output);
	static void WriteUnsignedIntegralData(uint columnSize, const std::string& dataStr, ByteStream& output);
	static void WriteFloatingPointData(uint columnSize, const std::string& dataStr, ByteStream& output);
	static void WriteStringData(uint columnSize, const std::string& dataStr, ByteStream& output);

private:
	std::string m_DatabasePath;
};