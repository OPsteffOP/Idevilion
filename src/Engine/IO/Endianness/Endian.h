// Folder: IO/Endianness

#pragma once

enum class Endian
{
	NATIVE,
	BIG_ENDIAN,
	LITTLE_ENDIAN
};

namespace EndianHelper
{
	inline constexpr bool IsSystemBigEndian()
	{
		union
		{
			uint16_t data;
			std::byte bytes[2];
		} test = { 0x0102 };

		return test.bytes[0] == (std::byte)1;
	}

	inline void SwitchEndian(const void* input, void* output, unsigned long long size)
	{
		std::reverse_copy((std::byte*)input, (std::byte*)input + size, (std::byte*)output);
	}

	template<typename T>
	inline void SwitchEndian(const T& input, T& output)
	{
		SwitchEndian((const void*)&input, (void*)&output, sizeof(T));
	}
}

template<typename T>
class BigEndianData
{
public:
	explicit BigEndianData(const T& data, Endian endian = Endian::NATIVE);

	const T& GetDataBigEndian() const { return this->data; }
	T GetNativeData() const;

private:
	union
	{
		T data;
		std::byte bytes[sizeof(T)];
	};
};

template<typename T>
BigEndianData<T>::BigEndianData(const T& data, Endian endian)
{
	if (endian == Endian::BIG_ENDIAN || (endian == Endian::NATIVE && EndianHelper::IsSystemBigEndian()))
	{
		this->data = data;
		return;
	}

	EndianHelper::SwitchEndian(data, this->data);
}

template<typename T>
T BigEndianData<T>::GetNativeData() const
{
	if (EndianHelper::IsSystemBigEndian())
		return this->data;

	T nativeData = this->data;
	EndianHelper::SwitchEndian(this->data, nativeData);
	return nativeData;
}