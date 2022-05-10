#include "DatabaseWriterPCH.h"
#include "DatabaseWriteDialog.h"

#include "FileDatabase.h"

#include <iostream>

void DatabaseWriteDialog::Start()
{
	RequestDatabasePath();

	std::function<void(DatabaseWriteDialog* pInstance)> operations[]
	{
		DatabaseOperation_AddColumn,
		DatabaseOperation_AddRow
	};

	int selectedOperation = -1;
	while ((selectedOperation = RequestDatabaseOperation()) != -1)
	{
		operations[selectedOperation](this);
	}
}

void DatabaseWriteDialog::RequestDatabasePath()
{
	std::cout << "Please specify the full database path (where to save / which one to modify):\n";
	std::getline(std::cin, m_DatabasePath);
	std::cout << "\n";

	SetConsoleTitle((std::wstring(L"Modifying database: ") + std::filesystem::path(m_DatabasePath).stem().wstring()).c_str());

	if (!std::filesystem::exists(m_DatabasePath))
	{
		ByteStream output(new ByteStreamBinaryFileImpl(m_DatabasePath, true));
		output.WriteUInt32(0);

		std::cout << "Database doesn't exist yet, will create one...\n\n";
	}
	else
	{
		std::cout << "Database does exist, opening for edit...\n\n";
	}
}

int DatabaseWriteDialog::RequestDatabaseOperation()
{
	std::cout << "Database operations:\n";
	std::cout << " -1 : Quit\n";
	std::cout << "  0 : Add Column\n";
	std::cout << "  1 : Add Row\n";
	std::cout << "> ";

	std::string selectedOperation = "";
	std::getline(std::cin, selectedOperation);

	std::cout << "\n";

	return std::stoi(selectedOperation);
}

void DatabaseWriteDialog::DatabaseOperation_AddColumn(DatabaseWriteDialog* pInstance)
{
	std::cout << "Column name\n";
	std::cout << "> ";
	std::string columnName = "";
	std::getline(std::cin, columnName);

	std::cout << "\nStore type\n";
	std::cout << " 1. Integral\n";
	std::cout << " 2. Unsigned Integral\n";
	std::cout << " 3. Floating-point\n";
	std::cout << " 4. String\n";
	std::cout << "> ";
	std::string storeTypeStr = "";
	std::getline(std::cin, storeTypeStr);

	std::cout << "\nMax data size\n";
	std::cout << "> ";
	std::string maxDataSizeStr = "";
	std::getline(std::cin, maxDataSizeStr);

	std::cout << "\nIs indexed\n";
	std::cout << "> ";
	std::string isIndexedStr = "";
	std::getline(std::cin, isIndexedStr);

	const DatabaseStoreType storeType = (DatabaseStoreType)std::stoi(storeTypeStr);
	const uint16_t maxDataSize = (uint16_t)std::stoul(maxDataSizeStr);
	const bool isIndexed = (isIndexedStr == "true" || isIndexedStr == "yes") ? true : false;

	if (!FileDatabaseHelper::IsValidType(storeType, maxDataSize))
	{
		std::cout << "\nFailed to add column, invalid Store type and Max data size combination\n\n";
		return;
	}

	FileDatabaseRowLayout layout;
	{
		ByteStream input(new ByteStreamBinaryFileImpl(pInstance->m_DatabasePath));
		layout.Load(input);
	}

	layout.AddColumn(columnName, storeType, maxDataSize, isIndexed);
	layout.Write(pInstance->m_DatabasePath);

	std::cout << "\n>> Added the column and saved the database\n\n";
}

void DatabaseWriteDialog::DatabaseOperation_AddRow(DatabaseWriteDialog* pInstance)
{
	FileDatabaseRowLayout layout;
	{
		ByteStream input(new ByteStreamBinaryFileImpl(pInstance->m_DatabasePath));
		layout.Load(input);
	}

	const std::unordered_map<std::string, FileDatabaseColumnLayout>& unsortedColumnLayouts = layout.GetColumnLayouts();
	if (unsortedColumnLayouts.empty())
	{
		std::cout << "Database doesn't have any columns yet, making it impossible to add a row\n\n";
		return;
	}

	std::vector<std::pair<std::string, FileDatabaseColumnLayout>> columnLayouts;
	for (const std::pair<std::string, FileDatabaseColumnLayout>& pair : unsortedColumnLayouts)
	{
		columnLayouts.emplace_back(pair.first, pair.second);
	}
	std::sort(columnLayouts.begin(), columnLayouts.end(), [](const std::pair<std::string, FileDatabaseColumnLayout>& pair1, const std::pair<std::string, FileDatabaseColumnLayout>& pair2)
		{
			return pair1.second.offset < pair2.second.offset;
		});

	std::cout << "Enter the row data:\n";

	std::vector<std::pair<std::string, std::string>> rowData;
	for (const std::pair<std::string, FileDatabaseColumnLayout>& pair : columnLayouts)
	{
		std::cout << " - " << pair.first << ": ";

		std::pair<std::string, std::string> dataPair = std::make_pair(pair.first, "");
		std::getline(std::cin, dataPair.second);
		rowData.push_back(dataPair);
	}

	const uintmax_t fileSize = std::filesystem::file_size(pInstance->m_DatabasePath);
	const uint rowIndex = (uint)((fileSize - layout.GetLayoutBlockSize()) / layout.GetRowSize());

	ByteStream output(new ByteStreamBinaryFileImpl(pInstance->m_DatabasePath));

	for (const std::pair<std::string, std::string>& pair : rowData)
	{
		const FileDatabaseColumnLayout& columnLayout = unsortedColumnLayouts.at(pair.first);

		const std::string parentPath = std::filesystem::path(pInstance->m_DatabasePath).parent_path().string();
		const std::string fileName = std::filesystem::path(pInstance->m_DatabasePath).stem().string();
		const std::string indexFilePath = std::filesystem::path(parentPath).append(fileName + "_" + pair.first + Paths::Data::Extensions::FILE_DATABASE_INDEX).string();

		switch (columnLayout.storeType)
		{
		case DatabaseStoreType::INTEGRAL:
		{
			WriteIntegralData(columnLayout.size, pair.second, output);

			if (columnLayout.isIndexed)
			{
				ByteStream indexOutput(new ByteStreamBinaryFileImpl(indexFilePath));
				WriteIntegralData(columnLayout.size, pair.second, indexOutput);
				indexOutput.WriteUInt32(rowIndex);
			}

			break;
		}
		case DatabaseStoreType::UNSIGNED_INTEGRAL:
		{
			WriteUnsignedIntegralData(columnLayout.size, pair.second, output);

			if (columnLayout.isIndexed)
			{
				ByteStream indexOutput(new ByteStreamBinaryFileImpl(indexFilePath));
				WriteUnsignedIntegralData(columnLayout.size, pair.second, indexOutput);
				indexOutput.WriteUInt32(rowIndex);
			}

			break;
		}
		case DatabaseStoreType::FLOATING_POINT:
		{
			WriteFloatingPointData(columnLayout.size, pair.second, output);

			if (columnLayout.isIndexed)
			{
				ByteStream indexOutput(new ByteStreamBinaryFileImpl(indexFilePath));
				WriteFloatingPointData(columnLayout.size, pair.second, indexOutput);
				indexOutput.WriteUInt32(rowIndex);
			}

			break;
		}
		case DatabaseStoreType::STRING:
		{
			WriteStringData(columnLayout.size, pair.second, output);

			if (columnLayout.isIndexed)
			{
				ByteStream indexOutput(new ByteStreamBinaryFileImpl(indexFilePath));
				WriteStringData(columnLayout.size, pair.second, indexOutput);
				indexOutput.WriteUInt32(rowIndex);
			}

			break;
		}
		}
	}

	std::cout << "\n>> Added the row and saved the database\n\n";
}

void DatabaseWriteDialog::WriteIntegralData(uint columnSize, const std::string& dataStr, ByteStream& output)
{
	if (columnSize != 1 && columnSize != 2 && columnSize != 4 && columnSize != 8)
	{
		// Database only supports integer sizes of 1, 2, 4, or 8
		DEBUG_ASSERT(false, "Should never hit here, database is corrupt");
	}

	const long long data = std::stoll(dataStr);
	output.WriteData(&data, columnSize, Endian::NATIVE);
}

void DatabaseWriteDialog::WriteUnsignedIntegralData(uint columnSize, const std::string& dataStr, ByteStream& output)
{
	if (columnSize != 1 && columnSize != 2 && columnSize != 4 && columnSize != 8)
	{
		// Database only supports unsigned integer sizes of 1, 2, 4, or 8
		DEBUG_ASSERT(false, "Should never hit here, database is corrupt");
	}

	const unsigned long long data = std::stoull(dataStr);
	output.WriteData(&data, columnSize, Endian::NATIVE);
}

void DatabaseWriteDialog::WriteFloatingPointData(uint columnSize, const std::string& dataStr, ByteStream& output)
{
	if (columnSize != 4 && columnSize != 8)
	{
		// Database only supports floating point sizes of 4 or 8
		DEBUG_ASSERT(false, "Should never hit here, database is corrupt");
	}

	const double data = std::stod(dataStr);
	output.WriteData(&data, columnSize, Endian::NATIVE);
}

void DatabaseWriteDialog::WriteStringData(uint columnSize, const std::string& dataStr, ByteStream& output)
{
	const uint strSize = std::clamp((uint)dataStr.length(), 0u, (uint)columnSize);
	const std::string truncStr(dataStr.c_str(), strSize);
	output.WriteString(truncStr);

	for (uint i = 0; i < columnSize - strSize; ++i)
	{
		output.WriteByte((std::byte)0);
	}
}