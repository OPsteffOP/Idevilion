// Folder: IO/Endianness

#include "EnginePCH.h"
#include "ByteStream.h"

ByteStream::ByteStream(ByteStreamImpl* pImpl)
	: m_pImpl(pImpl)
{}

ByteStream::~ByteStream()
{
	SAFE_DELETE(m_pImpl);
}

ByteStream::ByteStream(ByteStream&& stream) noexcept
	: m_pImpl(stream.m_pImpl)
{
	stream.m_pImpl = nullptr;
}

ByteStream& ByteStream::operator=(ByteStream&& stream) noexcept
{
	m_pImpl = stream.m_pImpl;
	stream.m_pImpl = nullptr;

	return *this;
}

void ByteStream::Reset()
{
	m_pImpl->Reset();
}

void ByteStream::WriteData(const void* pData, uint size, Endian fromEndian)
{
	WriteData(pData, (unsigned long long)size, fromEndian);
}

void ByteStream::WriteData(const void* pData, unsigned long long size, Endian fromEndian)
{
	if (size <= 1 || fromEndian == Endian::BIG_ENDIAN || (fromEndian == Endian::NATIVE && EndianHelper::IsSystemBigEndian()))
	{
		m_pImpl->Write(pData, size);
		return;
	}

	std::byte* pTemp = new std::byte[size];
	EndianHelper::SwitchEndian(pData, (void*)pTemp, size);
	m_pImpl->Write(pTemp, size);
	delete[] pTemp;
}

void ByteStream::ReadData(void* pData, uint size, Endian toEndian)
{
	ReadData(pData, (unsigned long long)size, toEndian);
}

void ByteStream::ReadData(void* pData, unsigned long long size, Endian toEndian)
{
	if (size <= 1 || toEndian == Endian::BIG_ENDIAN || (toEndian == Endian::NATIVE && EndianHelper::IsSystemBigEndian()))
	{
		m_pImpl->Read(pData, size);
		return;
	}

	std::byte* pTemp = new std::byte[size];
	m_pImpl->Read(pTemp, size);
	EndianHelper::SwitchEndian((void*)pTemp, pData, size);
	delete[] pTemp;
}

void ByteStream::WriteByte(std::byte data)
{
	m_pImpl->Write(&data, sizeof(data));
}

void ByteStream::WriteBool(bool data)
{
	m_pImpl->Write(&data, sizeof(data));
}

void ByteStream::WriteUChar(unsigned char data)
{
	m_pImpl->Write(&data, sizeof(data));
}

void ByteStream::WriteChar(char data)
{
	m_pImpl->Write(&data, sizeof(data));
}

void ByteStream::WriteInt8(int8_t data)
{
	m_pImpl->Write(&data, sizeof(data));
}

void ByteStream::WriteInt16(int16_t data)
{
	BigEndianData endian(data);
	int16_t endianData = endian.GetDataBigEndian();
	m_pImpl->Write(&endianData, sizeof(endianData));
}

void ByteStream::WriteInt32(int32_t data)
{
	BigEndianData endian(data);
	int32_t endianData = endian.GetDataBigEndian();
	m_pImpl->Write(&endianData, sizeof(endianData));
}

void ByteStream::WriteInt64(int64_t data)
{
	BigEndianData endian(data);
	int64_t endianData = endian.GetDataBigEndian();
	m_pImpl->Write(&endianData, sizeof(endianData));
}

void ByteStream::WriteUInt8(uint8_t data)
{
	m_pImpl->Write(&data, sizeof(data));
}

void ByteStream::WriteUInt16(uint16_t data)
{
	BigEndianData endian(data);
	uint16_t endianData = endian.GetDataBigEndian();
	m_pImpl->Write(&endianData, sizeof(endianData));
}

void ByteStream::WriteUInt32(uint32_t data)
{
	BigEndianData endian(data);
	uint32_t endianData = endian.GetDataBigEndian();
	m_pImpl->Write(&endianData, sizeof(endianData));
}

void ByteStream::WriteUInt64(uint64_t data)
{
	BigEndianData endian(data);
	uint64_t endianData = endian.GetDataBigEndian();
	m_pImpl->Write(&endianData, sizeof(endianData));
}

void ByteStream::WriteFloat(float data)
{
	BigEndianData endian(data);
	float endianData = endian.GetDataBigEndian();
	m_pImpl->Write(&endianData, sizeof(endianData));
}

void ByteStream::WriteDouble(double data)
{
	BigEndianData endian(data);
	double endianData = endian.GetDataBigEndian();
	m_pImpl->Write(&endianData, sizeof(endianData));
}

void ByteStream::WriteString(const std::string& data)
{
	m_pImpl->Write(data.c_str(), (uint)data.length());
}

void ByteStream::WritePoint2i(const Point2i& data)
{
	{
		BigEndianData endian(data.x);
		int endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}

	{
		BigEndianData endian(data.y);
		int endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}
}

void ByteStream::WritePoint2f(const Point2f& data)
{
	{
		BigEndianData endian(data.x);
		float endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}

	{
		BigEndianData endian(data.y);
		float endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}
}

void ByteStream::WriteRect4i(const Rect4i& data)
{
	{
		BigEndianData endian(data.x);
		int endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}

	{
		BigEndianData endian(data.y);
		int endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}

	{
		BigEndianData endian(data.width);
		int endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}

	{
		BigEndianData endian(data.height);
		int endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}
}

void ByteStream::WriteRect4f(const Rect4f& data)
{
	{
		BigEndianData endian(data.x);
		float endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}

	{
		BigEndianData endian(data.y);
		float endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}

	{
		BigEndianData endian(data.width);
		float endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}

	{
		BigEndianData endian(data.height);
		float endianData = endian.GetDataBigEndian();
		m_pImpl->Write(&endianData, sizeof(endianData));
	}
}

void ByteStream::WriteNetworkUUID(const NetworkUUID& data)
{
	{
		BigEndianData endianAddress(data.ipAddress);
		uint32_t endianAddressData = endianAddress.GetDataBigEndian();
		m_pImpl->Write(&endianAddressData, sizeof(endianAddressData));
	}

	{
		BigEndianData endianTimestamp(data.timestamp);
		uint64_t endianTimestampData = endianTimestamp.GetDataBigEndian();
		m_pImpl->Write(&endianTimestampData, sizeof(endianTimestampData));
	}

	{
		BigEndianData endianRandom(data.random);
		uint32_t endianRandomData = endianRandom.GetDataBigEndian();
		m_pImpl->Write(&endianRandomData, sizeof(endianRandomData));
	}
}

std::byte ByteStream::ReadByte() const
{
	std::byte data;
	m_pImpl->Read(&data, sizeof(data));
	return data;
}

bool ByteStream::ReadBool() const
{
	bool data;
	m_pImpl->Read(&data, sizeof(data));
	return data;
}

unsigned char ByteStream::ReadUChar() const
{
	unsigned char data;
	m_pImpl->Read(&data, sizeof(data));
	return data;
}

char ByteStream::ReadChar() const
{
	char data;
	m_pImpl->Read(&data, sizeof(data));
	return data;
}

int8_t ByteStream::ReadInt8() const
{
	int8_t data;
	m_pImpl->Read(&data, sizeof(data));
	return data;
}

int16_t ByteStream::ReadInt16() const
{
	int16_t data;

	m_pImpl->Read(&data, sizeof(data));
	BigEndianData endian(data, Endian::BIG_ENDIAN);
	data = endian.GetNativeData();

	return data;
}

int32_t ByteStream::ReadInt32() const
{
	int32_t data;

	m_pImpl->Read(&data, sizeof(data));
	BigEndianData endian(data, Endian::BIG_ENDIAN);
	data = endian.GetNativeData();

	return data;
}

int64_t ByteStream::ReadInt64() const
{
	int64_t data;

	m_pImpl->Read(&data, sizeof(data));
	BigEndianData endian(data, Endian::BIG_ENDIAN);
	data = endian.GetNativeData();

	return data;
}

uint8_t ByteStream::ReadUInt8() const
{
	uint8_t data;
	m_pImpl->Read(&data, sizeof(data));
	return data;
}

uint16_t ByteStream::ReadUInt16() const
{
	uint16_t data;

	m_pImpl->Read(&data, sizeof(data));
	BigEndianData endian(data, Endian::BIG_ENDIAN);
	data = endian.GetNativeData();

	return data;
}

uint32_t ByteStream::ReadUInt32() const
{
	uint32_t data;

	m_pImpl->Read(&data, sizeof(data));
	BigEndianData endian(data, Endian::BIG_ENDIAN);
	data = endian.GetNativeData();

	return data;
}

uint64_t ByteStream::ReadUInt64() const
{
	uint64_t data;

	m_pImpl->Read(&data, sizeof(data));
	BigEndianData endian(data, Endian::BIG_ENDIAN);
	data = endian.GetNativeData();

	return data;
}

float ByteStream::ReadFloat() const
{
	float data;

	m_pImpl->Read(&data, sizeof(data));
	BigEndianData endian(data, Endian::BIG_ENDIAN);
	data = endian.GetNativeData();

	return data;
}

double ByteStream::ReadDouble() const
{
	double data;

	m_pImpl->Read(&data, sizeof(data));
	BigEndianData endian(data, Endian::BIG_ENDIAN);
	data = endian.GetNativeData();

	return data;
}

std::string ByteStream::ReadString(uint size) const
{
	std::string data;

	char* temp = new char[size];
	m_pImpl->Read(temp, size);
	data = std::string(temp, size);
	delete temp;

	return data;
}

Point2i ByteStream::ReadPoint2i() const
{
	Point2i data;

	{
		m_pImpl->Read(&data.x, sizeof(data.x));
		BigEndianData endian(data.x, Endian::BIG_ENDIAN);
		data.x = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.y, sizeof(data.y));
		BigEndianData endian(data.y, Endian::BIG_ENDIAN);
		data.y = endian.GetNativeData();
	}

	return data;
}

Point2f ByteStream::ReadPoint2f() const
{
	Point2f data;

	{
		m_pImpl->Read(&data.x, sizeof(data.x));
		BigEndianData endian(data.x, Endian::BIG_ENDIAN);
		data.x = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.y, sizeof(data.y));
		BigEndianData endian(data.y, Endian::BIG_ENDIAN);
		data.y = endian.GetNativeData();
	}

	return data;
}

Rect4i ByteStream::ReadRect4i() const
{
	Rect4i data;

	{
		m_pImpl->Read(&data.x, sizeof(data.x));
		BigEndianData endian(data.x, Endian::BIG_ENDIAN);
		data.x = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.y, sizeof(data.y));
		BigEndianData endian(data.y, Endian::BIG_ENDIAN);
		data.y = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.width, sizeof(data.width));
		BigEndianData endian(data.width, Endian::BIG_ENDIAN);
		data.width = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.height, sizeof(data.height));
		BigEndianData endian(data.height, Endian::BIG_ENDIAN);
		data.height = endian.GetNativeData();
	}

	return data;
}

Rect4f ByteStream::ReadRect4f() const
{
	Rect4f data;

	{
		m_pImpl->Read(&data.x, sizeof(data.x));
		BigEndianData endian(data.x, Endian::BIG_ENDIAN);
		data.x = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.y, sizeof(data.y));
		BigEndianData endian(data.y, Endian::BIG_ENDIAN);
		data.y = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.width, sizeof(data.width));
		BigEndianData endian(data.width, Endian::BIG_ENDIAN);
		data.width = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.height, sizeof(data.height));
		BigEndianData endian(data.height, Endian::BIG_ENDIAN);
		data.height = endian.GetNativeData();
	}

	return data;
}

NetworkUUID ByteStream::ReadNetworkUUID() const
{
	NetworkUUID data;

	{
		m_pImpl->Read(&data.ipAddress, sizeof(data.ipAddress));
		BigEndianData endian(data.ipAddress, Endian::BIG_ENDIAN);
		data.ipAddress = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.timestamp, sizeof(data.timestamp));
		BigEndianData endian(data.timestamp, Endian::BIG_ENDIAN);
		data.timestamp = endian.GetNativeData();
	}

	{
		m_pImpl->Read(&data.random, sizeof(data.random));
		BigEndianData endian(data.random, Endian::BIG_ENDIAN);
		data.random = endian.GetNativeData();
	}

	return data;
}

ByteStreamBinaryFileImpl::ByteStreamBinaryFileImpl(const std::string& filePath, bool shouldTruncFile)
	: m_FilePath(filePath)
{
	if (shouldTruncFile)
	{
		m_Output.open(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
		m_Output.close();
	}
	else
	{
		if (std::filesystem::exists(filePath))
			m_WritePointer = std::filesystem::file_size(filePath);
	}
}

void ByteStreamBinaryFileImpl::Reset()
{
	std::unique_lock lock(m_FileAccessMutex);
	m_ReadPointer = 0;
}

void ByteStreamBinaryFileImpl::Write(const void* pData, unsigned long long size)
{
	std::unique_lock lock(m_FileAccessMutex);

	if (!m_Output.is_open())
	{
		if (m_Input.is_open())
			m_Input.close();

		m_Output.open(m_FilePath, std::ios::out | std::ios::app | std::ios::binary);
	}

	m_Output.seekp(m_WritePointer);
	m_Output.write((const char*)pData, size);
	m_WritePointer += size;
}

void ByteStreamBinaryFileImpl::Read(void* pData, unsigned long long size)
{
	DEBUG_ASSERT(m_ReadPointer + size <= std::filesystem::file_size(m_FilePath), "ByteStreamBinaryFileImpl trying to read past the file size");

	std::unique_lock lock(m_FileAccessMutex);

	if (!m_Input.is_open())
	{
		if (m_Output.is_open())
			m_Output.close();

		m_Input.open(m_FilePath, std::ios::in | std::ios::binary);
	}

	m_Input.seekg(m_ReadPointer);
	m_Input.read((char*)pData, size);
	m_ReadPointer += size;
}