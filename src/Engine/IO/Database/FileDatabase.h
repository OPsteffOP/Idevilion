// Folder: IO/Database

#pragma once
#include "ByteStream.h"

enum class DatabaseStoreType : unsigned char
{
	INTEGRAL = 1,
	UNSIGNED_INTEGRAL = 2,
	FLOATING_POINT = 3,
	STRING = 4
};

namespace FileDatabaseHelper
{
	bool IsValidType(DatabaseStoreType storeType, uint16_t size);
};

// --------------- FileDatabaseStaticString ---------------
template<size_t StringMaxSize>
class FileDatabaseStaticString
{
public:
	FileDatabaseStaticString() = default;
	FileDatabaseStaticString(const char* str);
	FileDatabaseStaticString(const std::string& str);

	bool operator==(const FileDatabaseStaticString<StringMaxSize>& other) const;
	bool operator!=(const FileDatabaseStaticString<StringMaxSize>& other) const;

	operator std::string() const;

	size_t GetActualSize() const;

public:
	constexpr static const size_t size = StringMaxSize;

private:
	char m_StringArray[size];
};

namespace std
{
	template <size_t StringMaxSize>
	struct hash<FileDatabaseStaticString<StringMaxSize>>
	{
		std::size_t operator()(const FileDatabaseStaticString<StringMaxSize>& str) const
		{
			return hash<string>()(str);
		}
	};
}

// --------------- FileDatabaseColumnLayout ---------------
PACK(1, struct FileDatabaseColumnLayout
{
	uint offset;
	DatabaseStoreType storeType;
	uint16_t size;
	bool isIndexed;
});

// --------------- FileDatabaseRowLayout ---------------
class FileDatabaseRowLayout
{
public:
	FileDatabaseRowLayout();

	void AddColumn(const std::string& columnName, DatabaseStoreType storeType, uint16_t size, bool isIndexed);

	void Load(ByteStream& input);
	void Write(const std::string& databasePath);

	template<typename T, typename = std::enable_if_t<std::is_pod_v<T>>> bool GetDataInColumn(const std::string& columnName, T& data, ByteStream& input) const;
	template<typename T, typename = std::enable_if_t<std::is_pod_v<T>>> bool GetDataInColumns(const std::vector<std::string>& columnNames, T& data, ByteStream& input) const;

	uint GetLayoutBlockSize() const { return m_LayoutBlockSize; }
	uint GetRowSize() const { return m_RowSize; }
	DatabaseStoreType GetColumnStoreType(const std::string& columnName) const;
	uint GetColumnSize(const std::string& columnName) const;
	bool IsColumnIndexed(const std::string& columnName) const;

	const std::unordered_map<std::string, FileDatabaseColumnLayout>& GetColumnLayouts() { return m_ColumnLayout; }

private:
	void GetDataInColumnInternal(const std::string& columnName, void* pData, ByteStream& input) const;
	bool ValidateGetDataInput(const std::string& columnName, ByteStream& input) const;

private:
	uint m_LayoutBlockSize;
	uint m_RowSize;
	std::unordered_map<std::string, FileDatabaseColumnLayout> m_ColumnLayout;

	std::vector<std::string> m_UnsavedColumnChanges;
};

// --------------- FileDatabaseIndex ---------------
template<typename T>
class FileDatabaseIndex
{
public:
	FileDatabaseIndex(const std::string& columnName);

	void Load(const std::string& indexFile, const FileDatabaseRowLayout& layout);

	// The first function will be called when the index is the same type as the primary key
	// If the first function gets called, it means that this index possibly is the one of the columnName, and possibly contains the primaryKeyValue
	// The second function will be called when the index is not the same type as the primary key
	// And thus the second function does nothing, it's just declared and implemented to satisfy the compiler
	template<typename U> typename std::enable_if_t<std::is_same_v<T, U>> GetRowIndexIfExist(const std::string& columnName, const U& primaryKeyValue, uint& index) const;
	template<typename U> typename std::enable_if_t<!std::is_same_v<T, U>> GetRowIndexIfExist(const std::string& columnName, const U& primaryKeyValue, uint& index) const;

	uint GetRowIndex(const T& primaryKeyValue) const;
	const std::string& GetColumnName() const { return m_ColumnName; }

public:
	constexpr static const uint INVALID_ROW_INDEX = UINT_MAX;

private:
	std::string m_ColumnName;

	// TODO: m_Index will need mutex when it's going to be modified - when adding the add/remove/modify functionality
	std::unordered_map<T, uint> m_Index;
};

// --------------- FileDatabase ---------------
template<typename... Indexes>
class FileDatabase
{
public:
	FileDatabase(const std::string& databaseFile, Indexes&&... indexes);

	template<typename KeyType, typename ValueType, typename = std::enable_if_t<std::is_pod_v<ValueType>&& std::is_pod_v<KeyType>>>
	bool GetDataValue(const std::string& primaryKeyName, const KeyType& primaryKeyValue, const std::string& columnName, ValueType& value);
	template<typename KeyType, typename ValueType, typename = std::enable_if_t<std::is_pod_v<ValueType>&& std::is_pod_v<KeyType>>>
	bool GetDataValues(const std::string& primaryKeyName, const KeyType& primaryKeyValue, const std::vector<std::string>& columnNames, ValueType& value);

private:
	unsigned long long GetRowPosition(uint rowIndex) const { return m_Layout.GetLayoutBlockSize() + (rowIndex * m_Layout.GetRowSize()); }

private:
	FileDatabaseRowLayout m_Layout;
	std::tuple<Indexes...> m_Indexes;

	ByteStream m_FileStream;
};

// --------------- FileDatabaseStaticString ---------------
template<size_t StringMaxSize>
FileDatabaseStaticString<StringMaxSize>::FileDatabaseStaticString(const char* str)
{
	if (strlen(str) > StringMaxSize)
	{
		std::string tempTruncatedStr = std::string(str, StringMaxSize);
		LOG_ERROR("Trying to initialize FileDatabaseStaticString with a string (%s) that is larger than the max size (%d), truncating to '%s'", str, (uint)StringMaxSize, tempTruncatedStr.c_str());
	}

	memset(m_StringArray, 0, StringMaxSize);
	const size_t actualSize = std::min(StringMaxSize, strlen(str));
	memcpy(m_StringArray, str, actualSize);
}

template<size_t StringMaxSize>
FileDatabaseStaticString<StringMaxSize>::FileDatabaseStaticString(const std::string& str)
{
	if (str.size() > StringMaxSize)
	{
		std::string tempTruncatedStr = std::string(str.c_str(), StringMaxSize);
		LOG_ERROR("Trying to initialize FileDatabaseStaticString with a string (%s) that is larger than the max size (%d), truncating to '%s'", str.c_str(), (uint)StringMaxSize, tempTruncatedStr.c_str());
	}

	memset(m_StringArray, 0, StringMaxSize);
	const size_t actualSize = std::min(StringMaxSize, str.size());
	memcpy(m_StringArray, str.c_str(), actualSize);
}

template<size_t StringMaxSize>
bool FileDatabaseStaticString<StringMaxSize>::operator==(const FileDatabaseStaticString<StringMaxSize>& other) const
{
	return std::memcmp(m_StringArray, other.m_StringArray, size) == 0;
}

template<size_t StringMaxSize>
bool FileDatabaseStaticString<StringMaxSize>::operator!=(const FileDatabaseStaticString<StringMaxSize>& other) const
{
	return !(*this == other);
}

template<size_t StringMaxSize>
FileDatabaseStaticString<StringMaxSize>::operator std::string() const
{
	return std::string(m_StringArray, size);
}

template<size_t StringMaxSize>
size_t FileDatabaseStaticString<StringMaxSize>::GetActualSize() const
{
	return std::min(strlen(m_StringArray), StringMaxSize);
}

// --------------- FileDatabaseRowLayout ---------------
template<typename T, typename>
bool FileDatabaseRowLayout::GetDataInColumn(const std::string& columnName, T& data, ByteStream& input) const
{
	if (!ValidateGetDataInput(columnName, input))
		return false;

	const FileDatabaseColumnLayout& columnLayout = m_ColumnLayout.at(columnName);
	if (sizeof(T) != columnLayout.size)
	{
		LOG_ERROR("Trying to get data in column out of file database, but sizeof(T) is not equal to the column size specified in the layout, %d != %d", (uint)sizeof(T), columnLayout.size);
		return false;
	}

	GetDataInColumnInternal(columnName, reinterpret_cast<void*>(&data), input);
	return true;
}

template<typename T, typename>
bool FileDatabaseRowLayout::GetDataInColumns(const std::vector<std::string>& columnNames, T& data, ByteStream& input) const
{
	// Validation
	uint totalSize = 0;
	for (const std::string& columnName : columnNames)
	{
		if (!ValidateGetDataInput(columnName, input))
			return false;

		const FileDatabaseColumnLayout& columnLayout = m_ColumnLayout.at(columnName);
		totalSize += columnLayout.size;
	}

	if (sizeof(T) != totalSize)
	{
		LOG_ERROR("Trying to get data in multiple columns out of file database, but sizeof(T) is not equal to the the sum of all requested column sizes specified in the layout, %d != %d", (uint)sizeof(T), totalSize);
		return false;
	}

	// Getting the data out of the database
	const long long rowStartPosition = input.GetReadBufferPointer();

	uint currentOffset = 0;
	for (const std::string& columnName : columnNames)
	{
		input.SetReadBufferPointer(rowStartPosition);

		GetDataInColumnInternal(columnName, reinterpret_cast<void*>(reinterpret_cast<char*>(&data) + currentOffset), input);
		currentOffset += m_ColumnLayout.at(columnName).size;
	}

	return true;
}

// --------------- FileDatabaseIndex ---------------
template<typename T>
FileDatabaseIndex<T>::FileDatabaseIndex(const std::string& columnName)
	: m_ColumnName(columnName)
{}

template<typename T>
void FileDatabaseIndex<T>::Load(const std::string& indexFile, const FileDatabaseRowLayout& layout)
{
	if (!std::filesystem::exists(indexFile))
	{
		LOG_ERROR("No index file found for file database with indexed_primary_key=%s at file_location=%s", m_ColumnName.c_str(), indexFile.c_str());
		return;
	}

	if (!layout.IsColumnIndexed(m_ColumnName))
	{
		LOG_ERROR("Trying to load a FileDatabaseIndex while the layout says the column isn't indexed, column_name=%s", m_ColumnName.c_str());
		return;
	}

	if (layout.GetColumnSize(m_ColumnName) != sizeof(T))
	{
		LOG_ERROR("Trying to load a FileDatabaseIndex with a column size that isn't equal to the column size specified in the layout, column_name=%s", m_ColumnName.c_str());
		return;
	}

	ByteStream input(new ByteStreamBinaryFileImpl(indexFile));

	const uintmax_t fileSize = std::filesystem::file_size(indexFile);
	while ((uintmax_t)input.GetReadBufferPointer() < fileSize)
	{
		T primaryKey;
		if (layout.GetColumnStoreType(m_ColumnName) == DatabaseStoreType::STRING)
			input.ReadData(&primaryKey, sizeof(primaryKey), Endian::BIG_ENDIAN); // Setting to BIG_ENDIAN because strings shouldn't be converted to the native endian
		else
			input.ReadData(&primaryKey, sizeof(primaryKey), Endian::NATIVE);

		uint rowIndex = input.ReadUInt32();

		m_Index.emplace(primaryKey, rowIndex);
	}
}

template<typename T>
template<typename U>
typename std::enable_if_t<std::is_same_v<T, U>> FileDatabaseIndex<T>::GetRowIndexIfExist(const std::string& columnName, const U& primaryKeyValue, uint& index) const
{
	if (m_ColumnName != columnName)
		return;

	const uint foundRowIndex = GetRowIndex(primaryKeyValue);
	if (foundRowIndex != INVALID_ROW_INDEX)
		index = foundRowIndex;
}

template<typename T>
template<typename U>
typename std::enable_if_t<!std::is_same_v<T, U>> FileDatabaseIndex<T>::GetRowIndexIfExist(const std::string& columnName, const U& primaryKeyValue, uint& index) const
{
	UNREFERENCED_PARAMETER(columnName);
	UNREFERENCED_PARAMETER(primaryKeyValue);
	UNREFERENCED_PARAMETER(index);
}

template<typename T>
uint FileDatabaseIndex<T>::GetRowIndex(const T& primaryKeyValue) const
{
	if (m_Index.find(primaryKeyValue) == m_Index.end())
		return INVALID_ROW_INDEX;

	return m_Index.at(primaryKeyValue);
}

// --------------- FileDatabase ---------------
template<typename... Indexes>
FileDatabase<Indexes...>::FileDatabase(const std::string& databaseFile, Indexes&&... indexes)
	: m_FileStream(new ByteStreamBinaryFileImpl(databaseFile))
	, m_Indexes(std::make_tuple(indexes...))
{
	// Load layout
	m_Layout.Load(m_FileStream);

	// Load indexes
	const std::filesystem::path databasePath = std::filesystem::path(databaseFile);
	const std::string parentPath = databasePath.parent_path().string();
	const std::string fileName = databasePath.stem().string();

	std::apply([this, &parentPath, &fileName](auto&... indexElements)
		{
			if (!(m_Layout.IsColumnIndexed(indexElements.GetColumnName()) && ...))
			{
				LOG_ERROR("A column shouldn't be indexed in the file database according to the layout block, FileDatabase templates are incorrect");
				return;
			}

			(indexElements.Load(std::filesystem::path(parentPath).append(fileName + "_" + indexElements.GetColumnName() + Paths::Data::Extensions::FILE_DATABASE_INDEX).string(), m_Layout), ...);
		}
	, m_Indexes);
}

template<typename... Indexes>
template<typename KeyType, typename ValueType, typename>
bool FileDatabase<Indexes...>::GetDataValue(const std::string& primaryKeyName, const KeyType& primaryKeyValue, const std::string& columnName, ValueType& value)
{
	if (!m_Layout.IsColumnIndexed(primaryKeyName))
		LOG_ERROR("Trying to retrieve a value of a primary key, but the primary key column isn't indexed in a file database, primary_key_name=%s", primaryKeyName.c_str());

	uint rowIndex = FileDatabaseIndex<KeyType>::INVALID_ROW_INDEX;
	std::apply([&rowIndex, &primaryKeyName, &primaryKeyValue](auto&... indexElements)
		{
			(indexElements.GetRowIndexIfExist(primaryKeyName, primaryKeyValue, rowIndex), ...);
		}
	, m_Indexes);

	if (rowIndex == FileDatabaseIndex<KeyType>::INVALID_ROW_INDEX)
		return false;

	const unsigned long long rowFilePosition = GetRowPosition(rowIndex);
	m_FileStream.SetReadBufferPointer(rowFilePosition);

	if (!m_Layout.GetDataInColumn<ValueType>(columnName, value, m_FileStream))
		return false;

	return true;
}

template<typename... Indexes>
template<typename KeyType, typename ValueType, typename>
bool FileDatabase<Indexes...>::GetDataValues(const std::string& primaryKeyName, const KeyType& primaryKeyValue, const std::vector<std::string>& columnNames, ValueType& value)
{
	if (!m_Layout.IsColumnIndexed(primaryKeyName))
		LOG_ERROR("Trying to retrieve a value of a primary key, but the primary key column isn't indexed in a file database, primary_key_name=%s", primaryKeyName.c_str());

	uint rowIndex = FileDatabaseIndex<KeyType>::INVALID_ROW_INDEX;
	std::apply([&rowIndex, &primaryKeyName, &primaryKeyValue](auto&... indexElements)
		{
			(indexElements.GetRowIndexIfExist(primaryKeyName, primaryKeyValue, rowIndex), ...);
		}
	, m_Indexes);

	if (rowIndex == FileDatabaseIndex<KeyType>::INVALID_ROW_INDEX)
		return false;

	const unsigned long long rowFilePosition = GetRowPosition(rowIndex);
	m_FileStream.SetReadBufferPointer(rowFilePosition);

	if (!m_Layout.GetDataInColumns<ValueType>(columnNames, value, m_FileStream))
		return false;

	return true;
}