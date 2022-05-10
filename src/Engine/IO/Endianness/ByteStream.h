// Folder: IO/Endianness

#pragma once
#include "Endian.h"

class ByteStreamImpl
{
	friend class ByteStream;

public:
	virtual ~ByteStreamImpl() = default;

	virtual void Reset() = 0;

	virtual void Write(const void* pData, unsigned long long size) = 0;
	virtual void Read(void* pData, unsigned long long size) = 0;

protected:
	unsigned long long m_WritePointer;
	unsigned long long m_ReadPointer;
};

class ByteStream
{
public:
	explicit ByteStream(ByteStreamImpl* pImpl);
	~ByteStream();

	ByteStream(const ByteStream& stream) = delete;
	ByteStream(ByteStream&& stream) noexcept;
	ByteStream& operator=(const ByteStream& stream) = delete;
	ByteStream& operator=(ByteStream&& stream) noexcept;

	void Reset();

	// Don't use these to read a whole struct, always read the individual values
	// The bytes will be reverted if the endian doesn't match
	// Meaning that when writing / reading a whole struct, the variable order won't match anymore
	void WriteData(const void* pData, uint size, Endian fromEndian);
	void WriteData(const void* pData, unsigned long long size, Endian fromEndian);
	void ReadData(void* pData, uint size, Endian toEndian);
	void ReadData(void* pData, unsigned long long size, Endian toEndian);

	void WriteByte(std::byte data);
	void WriteBool(bool data);
	void WriteUChar(unsigned char data);
	void WriteChar(char data);
	void WriteInt8(int8_t data);
	void WriteInt16(int16_t data);
	void WriteInt32(int32_t data);
	void WriteInt64(int64_t data);
	void WriteUInt8(uint8_t data);
	void WriteUInt16(uint16_t data);
	void WriteUInt32(uint32_t data);
	void WriteUInt64(uint64_t data);
	void WriteFloat(float data);
	void WriteDouble(double data);
	void WriteString(const std::string& data);
	void WritePoint2i(const Point2i& data);
	void WritePoint2f(const Point2f& data);
	void WriteRect4f(const Rect4f& data);
	void WriteNetworkUUID(const NetworkUUID& data);

	std::byte ReadByte() const;
	bool ReadBool() const;
	unsigned char ReadUChar() const;
	char ReadChar() const;
	int8_t ReadInt8() const;
	int16_t ReadInt16() const;
	int32_t ReadInt32() const;
	int64_t ReadInt64() const;
	uint8_t ReadUInt8() const;
	uint16_t ReadUInt16() const;
	uint32_t ReadUInt32() const;
	uint64_t ReadUInt64() const;
	float ReadFloat() const;
	double ReadDouble() const;
	std::string ReadString(uint size) const;
	Point2i ReadPoint2i() const;
	Point2f ReadPoint2f() const;
	Rect4f ReadRect4f() const;
	NetworkUUID ReadNetworkUUID() const;

	ByteStreamImpl* GetImpl() const { return m_pImpl; }

	unsigned long long GetWriteBufferPointer() const { return m_pImpl->m_WritePointer; }
	void SetWriteBufferPointer(unsigned long long pointer) const { m_pImpl->m_WritePointer = pointer; }
	unsigned long long GetReadBufferPointer() const { return m_pImpl->m_ReadPointer; }
	void SetReadBufferPointer(unsigned long long pointer) const { m_pImpl->m_ReadPointer = pointer; }

private:
	ByteStreamImpl* m_pImpl;
};

class ByteStreamBinaryFileImpl : public ByteStreamImpl
{
public:
	ByteStreamBinaryFileImpl(const std::string& filePath, bool shouldTruncFile = false);

	virtual void Reset() override;

	virtual void Write(const void* pData, unsigned long long size) override;
	virtual void Read(void* pData, unsigned long long size) override;

private:
	const std::string m_FilePath;

	std::mutex m_FileAccessMutex;
	std::ifstream m_Input;
	std::ofstream m_Output;
};

template<unsigned long long Capacity>
class ByteStreamBufferImpl : public ByteStreamImpl
{
public:
	virtual void Reset() override;

	virtual void Write(const void* pData, unsigned long long size) override;
	virtual void Read(void* pData, unsigned long long size) override;

	const std::byte* GetBuffer() const { return m_Buffer; }

private:
	std::byte m_Buffer[Capacity];
};

template<unsigned long long Capacity>
void ByteStreamBufferImpl<Capacity>::Reset()
{
	m_WritePointer = 0;
	m_ReadPointer = 0;
}

template<unsigned long long Capacity>
void ByteStreamBufferImpl<Capacity>::Write(const void* pData, unsigned long long size)
{
	DEBUG_ASSERT(m_WritePointer + size <= Capacity, "ByteStreamBufferImpl trying to write past the capacity");

	std::copy((std::byte*)pData, (std::byte*)pData + size, m_Buffer + m_WritePointer);
	m_WritePointer += size;
}

template<unsigned long long Capacity>
void ByteStreamBufferImpl<Capacity>::Read(void* pData, unsigned long long size)
{
	DEBUG_ASSERT(m_ReadPointer + size <= m_WritePointer, "ByteStreamBufferImpl trying to read past the write buffer pointer");

	std::copy(m_Buffer + m_ReadPointer, m_Buffer + m_ReadPointer + size, (std::byte*)pData);
	m_ReadPointer += size;
}