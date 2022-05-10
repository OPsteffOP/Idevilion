// Folder: IO/Database

#include "EnginePCH.h"
#include "FileDatabase.h"

bool FileDatabaseHelper::IsValidType(DatabaseStoreType storeType, uint16_t size)
{
	switch (storeType)
	{
	case DatabaseStoreType::INTEGRAL:
	case DatabaseStoreType::UNSIGNED_INTEGRAL:
		return size == 1 || size == 2 || size == 4 || size == 8;
	case DatabaseStoreType::FLOATING_POINT:
		return size == 4 || size == 8;
	case DatabaseStoreType::STRING:
		return size > 0;
	}

	return false;
}

// --------------- FileDatabaseRowLayout ---------------
FileDatabaseRowLayout::FileDatabaseRowLayout()
	: m_LayoutBlockSize(0)
	, m_RowSize(0)
	, m_ColumnLayout()
{}

void FileDatabaseRowLayout::AddColumn(const std::string& columnName, DatabaseStoreType storeType, uint16_t size, bool isIndexed)
{
	uint offset = 0;
	for (const std::pair<std::string, FileDatabaseColumnLayout>& pair : m_ColumnLayout)
	{
		if (offset <= pair.second.offset + pair.second.size)
			offset = pair.second.offset + pair.second.size;
	}

	FileDatabaseColumnLayout columnLayout;
	columnLayout.offset = offset;
	columnLayout.storeType = storeType;
	columnLayout.size = size;
	columnLayout.isIndexed = isIndexed;

	m_ColumnLayout.emplace(columnName, columnLayout);
	m_UnsavedColumnChanges.push_back(columnName);
}

void FileDatabaseRowLayout::Load(ByteStream& input)
{
	if (input.GetReadBufferPointer() != 0)
	{
		LOG_WARNING("Trying to load file database layout, but the ByteStream isn't pointing at the start of the file");
		input.SetReadBufferPointer(0);
	}

	m_LayoutBlockSize = input.ReadUInt32();

	uint currentPos = 0;
	while (currentPos != m_LayoutBlockSize)
	{
		const uint8_t columnNameLength = input.ReadUInt8();
		const std::string columnName = input.ReadString(columnNameLength);

		const uint columnOffset = input.ReadUInt32();
		const DatabaseStoreType storeType = (DatabaseStoreType)input.ReadUChar();
		const uint16_t columnSize = input.ReadUInt16();
		const bool isIndexed = input.ReadBool();

		FileDatabaseColumnLayout& columnLayout = m_ColumnLayout[columnName];
		columnLayout.offset = columnOffset;
		columnLayout.storeType = storeType;
		columnLayout.size = columnSize;
		columnLayout.isIndexed = isIndexed;

		m_RowSize = std::max(m_RowSize, columnOffset + columnSize);

		currentPos += sizeof(columnNameLength) + columnNameLength + sizeof(columnOffset) + sizeof(storeType) + sizeof(columnSize) + sizeof(isIndexed);
	}

	m_LayoutBlockSize += sizeof(uint);
}

void FileDatabaseRowLayout::Write(const std::string& databasePath)
{
	const std::string tempFilePath = std::filesystem::path(Paths::OS::GetLocalAppFolder()).append(std::string("Temp\\temp_db") + Paths::Data::Extensions::FILE_DATABASE).string();

	{
		ByteStream input(new ByteStreamBinaryFileImpl(databasePath));
		ByteStream output(new ByteStreamBinaryFileImpl(tempFilePath, true));

		std::vector<std::pair<std::string, FileDatabaseColumnLayout>> layout;
		for (const std::pair<std::string, FileDatabaseColumnLayout>& pair : m_ColumnLayout)
		{
			layout.emplace_back(pair.first, pair.second);
		}
		std::sort(layout.begin(), layout.end(), [](const std::pair<std::string, FileDatabaseColumnLayout>& pair1, const std::pair<std::string, FileDatabaseColumnLayout>& pair2)
			{
				return pair1.second.offset < pair2.second.offset;
			});

		// Writing layout size
		uint totalLayoutSize = 0;
		for (const std::pair<std::string, FileDatabaseColumnLayout>& pair : layout)
		{
			totalLayoutSize += (uint)(sizeof(std::byte) + pair.first.length() + sizeof(FileDatabaseColumnLayout));
		}
		output.WriteUInt32(totalLayoutSize);

		// Writing layout
		for (const std::pair<std::string, FileDatabaseColumnLayout>& pair : layout)
		{
			uint8_t columnNameLength = (uint8_t)pair.first.length();
			output.WriteUInt8(columnNameLength);
			output.WriteString(pair.first);

			output.WriteUInt32(pair.second.offset);
			output.WriteUChar((unsigned char)pair.second.storeType);
			output.WriteUInt16(pair.second.size);
			output.WriteBool(pair.second.isIndexed);
		}

		// Writing data
		const uint layoutSize = input.ReadUInt32();
		input.SetReadBufferPointer(sizeof(layoutSize) + layoutSize);

		std::unordered_map<std::string, ByteStream> indexFiles;
		for (const std::string& columnName : m_UnsavedColumnChanges)
		{
			if (!m_ColumnLayout[columnName].isIndexed)
				continue;

			const std::string parentPath = std::filesystem::path(databasePath).parent_path().string();
			const std::string fileName = std::filesystem::path(databasePath).stem().string();

			const std::string indexFilePath = std::filesystem::path(parentPath).append(fileName + "_" + columnName + Paths::Data::Extensions::FILE_DATABASE_INDEX).string();
			indexFiles.emplace(columnName, ByteStream(new ByteStreamBinaryFileImpl(indexFilePath, true)));
		}

		const uintmax_t fileSize = std::filesystem::file_size(databasePath);
		uint rowIndex = 0;
		while ((uintmax_t)input.GetReadBufferPointer() < fileSize)
		{
			for (const std::pair<std::string, FileDatabaseColumnLayout>& pair : layout)
			{
				if (std::find(m_UnsavedColumnChanges.begin(), m_UnsavedColumnChanges.end(), pair.first) != m_UnsavedColumnChanges.end())
				{
					for (uint i = 0; i < pair.second.size; ++i)
					{
						output.WriteByte((std::byte)0);
					}

					if (pair.second.isIndexed)
					{
						ByteStream& indexOutput = indexFiles.at(pair.first);

						for (uint i = 0; i < pair.second.size; ++i)
						{
							indexOutput.WriteByte((std::byte)0);
						}

						indexOutput.WriteUInt32(rowIndex);
					}

					continue;
				}

				char* columnData = new char[pair.second.size];
				input.ReadData(columnData, (uint)pair.second.size, Endian::BIG_ENDIAN);
				output.WriteData(columnData, (uint)pair.second.size, Endian::BIG_ENDIAN);
				delete[] columnData;
			}

			++rowIndex;
		}
	}

	std::filesystem::copy_file(tempFilePath, databasePath, std::filesystem::copy_options::overwrite_existing);
	m_UnsavedColumnChanges.clear();
}

void FileDatabaseRowLayout::GetDataInColumnInternal(const std::string& columnName, void* pData, ByteStream& input) const
{
	const FileDatabaseColumnLayout& columnLayout = m_ColumnLayout.at(columnName);
	input.SetReadBufferPointer(input.GetReadBufferPointer() + columnLayout.offset);

	if (columnLayout.storeType == DatabaseStoreType::STRING)
		input.ReadData(pData, (uint)columnLayout.size, Endian::BIG_ENDIAN); // Setting to BIG_ENDIAN because strings shouldn't be converted to the native endian
	else
		input.ReadData(pData, (uint)columnLayout.size, Endian::NATIVE);
}

bool FileDatabaseRowLayout::ValidateGetDataInput(const std::string& columnName, ByteStream& input) const
{
	if (m_ColumnLayout.find(columnName) == m_ColumnLayout.end())
	{
		LOG_ERROR("Trying to get data in column out of a file database but the layout doesn't contain a column with name=%s", columnName.c_str());
		return false;
	}

	if (input.GetReadBufferPointer() < m_LayoutBlockSize)
	{
		LOG_ERROR("Trying to get data in column out of file database, but the ByteStream is pointing somewhere in the layout block");
		return false;
	}

	if ((input.GetReadBufferPointer() - (std::streampos)m_LayoutBlockSize) % m_RowSize != 0)
	{
		LOG_ERROR("Trying to get data in column out of file database, but the ByteStream isn't pointing to the beginning of a row");
		return false;
	}

	return true;
}

DatabaseStoreType FileDatabaseRowLayout::GetColumnStoreType(const std::string& columnName) const
{
	if (m_ColumnLayout.find(columnName) == m_ColumnLayout.end())
	{
		LOG_ERROR("Trying to get column store type in file database, but a column with name=%s doesn't exist", columnName.c_str());
		return DatabaseStoreType(-1);
	}

	return m_ColumnLayout.at(columnName).storeType;
}

uint FileDatabaseRowLayout::GetColumnSize(const std::string& columnName) const
{
	if (m_ColumnLayout.find(columnName) == m_ColumnLayout.end())
	{
		LOG_ERROR("Trying to get column size in file database, but a column with name=%s doesn't exist", columnName.c_str());
		return 0;
	}

	return m_ColumnLayout.at(columnName).size;
}

bool FileDatabaseRowLayout::IsColumnIndexed(const std::string& columnName) const
{
	if (m_ColumnLayout.find(columnName) == m_ColumnLayout.end())
	{
		LOG_ERROR("Trying to check whether a column is indexed in file database, but a column with name=%s doesn't exist", columnName.c_str());
		return false;
	}

	return m_ColumnLayout.at(columnName).isIndexed;
}