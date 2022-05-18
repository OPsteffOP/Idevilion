// Folder: IO/Devices

#include "EnginePCH.h"
#include "DirectInput.h"

#ifdef BP_PLATFORM_WINDOWS

#include <wbemidl.h>
#include <oleauto.h>

DirectInput::DirectInput()
{
	HRESULT result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	if (FAILED(result))
	{
		LOG_ERROR("Failed to initialize DirectInput, error_code=%d", result);
		m_pDirectInput = nullptr;
		return;
	}
}

const std::unordered_set<DirectInput::ControllerData, DirectInput::ControllerData::Hasher>& DirectInput::EnumerateDevices()
{
	m_FoundControllerData.clear();

	if (m_pDirectInput == nullptr)
		return m_FoundControllerData;

	HRESULT result = m_pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumerateDevicesCallback, &m_FoundControllerData, DIEDFL_ATTACHEDONLY);
	if (FAILED(result))
	{
		LOG_ERROR("DirectInput failed to enumerate devices, error_code=%d", result);
		return m_FoundControllerData;
	}

	return m_FoundControllerData;
}

BOOL DirectInput::EnumerateDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	std::unordered_set<ControllerData, DirectInput::ControllerData::Hasher>& foundControllerData = *static_cast<std::unordered_set<ControllerData, DirectInput::ControllerData::Hasher>*>(pvRef);

	ControllerData data;
	data.instanceGUID = lpddi->guidInstance;
	data.productGUID = lpddi->guidProduct;
	data.driverType = ControllerData::DriverType::DIRECT_INPUT;

	if (foundControllerData.find(data) != foundControllerData.end())
		return DIENUM_CONTINUE;

	if (IsXInputDevice(data))
		data.driverType = ControllerData::DriverType::XINPUT;
	else if (IsScePadDevice(data))
		data.driverType = ControllerData::DriverType::SCEPAD;

	foundControllerData.emplace(data);
	return DIENUM_CONTINUE;
}

bool DirectInput::IsXInputDevice(const ControllerData& data)
{
	// https://docs.microsoft.com/en-us/windows/win32/xinput/xinput-and-directinput

	IWbemLocator* pIWbemLocator = nullptr;
	IEnumWbemClassObject* pEnumDevices = nullptr;
	IWbemClassObject* pDevices[20] = {};
	IWbemServices* pIWbemServices = nullptr;
	BSTR bstrNamespace = nullptr;
	BSTR bstrDeviceID = nullptr;
	BSTR bstrClassName = nullptr;
	bool bIsXinputDevice = false;

	// CoInit if needed
	HRESULT hr = CoInitialize(nullptr);
	bool bCleanupCOM = SUCCEEDED(hr);

	// So we can call VariantClear() later, even if we never had a successful IWbemClassObject::Get().
	VARIANT var = {};
	VariantInit(&var);

	// Create WMI
	hr = CoCreateInstance(__uuidof(WbemLocator),
		nullptr,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWbemLocator),
		(LPVOID*)&pIWbemLocator);
	if (FAILED(hr) || pIWbemLocator == nullptr)
		goto LCleanup;

	bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");  if (bstrNamespace == nullptr) goto LCleanup;
	bstrClassName = SysAllocString(L"Win32_PNPEntity");     if (bstrClassName == nullptr) goto LCleanup;
	bstrDeviceID = SysAllocString(L"DeviceID");             if (bstrDeviceID == nullptr)  goto LCleanup;

	// Connect to WMI 
	hr = pIWbemLocator->ConnectServer(bstrNamespace, nullptr, nullptr, 0L,
		0L, nullptr, nullptr, &pIWbemServices);
	if (FAILED(hr) || pIWbemServices == nullptr)
		goto LCleanup;

	// Switch security level to IMPERSONATE. 
	hr = CoSetProxyBlanket(pIWbemServices,
		RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
		RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr, EOAC_NONE);
	if (FAILED(hr))
		goto LCleanup;

	hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, nullptr, &pEnumDevices);
	if (FAILED(hr) || pEnumDevices == nullptr)
		goto LCleanup;

	// Loop over all devices
	for (;;)
	{
		ULONG uReturned = 0;
		hr = pEnumDevices->Next(10000, _countof(pDevices), pDevices, &uReturned);
		if (FAILED(hr))
			goto LCleanup;
		if (uReturned == 0)
			break;

		for (size_t iDevice = 0; iDevice < uReturned; ++iDevice)
		{
			// For each device, get its device ID
			hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, nullptr, nullptr);
			if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr)
			{
				// Check if the device ID contains "IG_".  If it does, then it's an XInput device
				// This information can not be found from DirectInput 
				if (wcsstr(var.bstrVal, L"IG_"))
				{
					// If it does, then get the VID/PID from var.bstrVal
					DWORD dwPid = 0, dwVid = 0;
					WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
					if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1)
						dwVid = 0;
					WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
					if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1)
						dwPid = 0;

					// Compare the VID/PID to the DInput device
					DWORD dwVidPid = MAKELONG(dwVid, dwPid);
					if (dwVidPid == data.productGUID.Data1)
					{
						bIsXinputDevice = true;
						goto LCleanup;
					}
				}
			}
			VariantClear(&var);
			SAFE_RELEASE(pDevices[iDevice]);
		}
	}

LCleanup:
	VariantClear(&var);

	if (bstrNamespace)
		SysFreeString(bstrNamespace);
	if (bstrDeviceID)
		SysFreeString(bstrDeviceID);
	if (bstrClassName)
		SysFreeString(bstrClassName);

	for (size_t iDevice = 0; iDevice < _countof(pDevices); ++iDevice)
		SAFE_RELEASE(pDevices[iDevice]);

	SAFE_RELEASE(pEnumDevices);
	SAFE_RELEASE(pIWbemLocator);
	SAFE_RELEASE(pIWbemServices);

	if (bCleanupCOM)
		CoUninitialize();

	return bIsXinputDevice;
}

bool DirectInput::IsScePadDevice(const ControllerData& data)
{
	switch (data.productGUID.Data1)
	{
	case 0x0ba0054c: // Sony DualShock 4 Wireless Adaptor
	case 0x05C4054C: // Sony DualShock 4
	case 0x09cc054c: // Sony DualShock 4 V2
	case 0x0ce6054c: // Sony DualSense Wireless
		return true;
	}

	return false;
}

#endif