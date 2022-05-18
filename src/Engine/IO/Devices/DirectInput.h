// Folder: IO/Devices

#pragma once
#ifdef BP_PLATFORM_WINDOWS

class DirectInput
{
public:
	struct ControllerData
	{
		enum class DriverType
		{
			DIRECT_INPUT,
			XINPUT,
			SCEPAD
		};

		GUID instanceGUID;
		GUID productGUID;
		DriverType driverType;

		bool operator==(const ControllerData& rhs) const
		{
			return instanceGUID == rhs.instanceGUID;
		}

		struct Hasher
		{
			size_t operator()(const ControllerData& data) const
			{
				size_t hash = 0;
				Utils::HashCombine(hash, data.instanceGUID.Data1);
				Utils::HashCombine(hash, data.instanceGUID.Data2);
				Utils::HashCombine(hash, data.instanceGUID.Data3);

				for (uint i = 0; i < 8; ++i)
				{
					Utils::HashCombine(hash, data.instanceGUID.Data4[i]);
				}

				return hash;
			}
		};
	};

public:
	DirectInput();

	const std::unordered_set<ControllerData, ControllerData::Hasher>& EnumerateDevices();

private:
	static BOOL EnumerateDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	static bool IsXInputDevice(const ControllerData& data);
	static bool IsScePadDevice(const ControllerData& data);

private:
	IDirectInput8* m_pDirectInput;

	std::unordered_set<ControllerData, ControllerData::Hasher> m_FoundControllerData;
};

#endif