// Folder: IO/Devices

#pragma once
#include "InputDevice.h"

#ifdef BP_PLATFORM_WINDOWS
#include "DirectInput.h"
#endif

class InputManager
{
public:
	static InputManager* GetInstance();
	~InputManager();

	void Update();

	InputDevice* GetDevice(InputDeviceIdentifier type) const;
	void SetDevice(InputDevice* pDevice);

	bool IsExclusiveInput(InputDeviceIdentifier type) const;
	ExclusiveInputDevice* ClaimExclusiveInput(InputDeviceIdentifier type);
	void ReleaseExclusiveInput(ExclusiveInputDevice* pDevice);

#ifdef BP_PLATFORM_WINDOWS
	static void WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void ForceScanControllersOnUpdate() { m_ShouldScanControllers = true; }
#endif

private:
	InputManager();

	void CreateDevices();

#ifdef BP_PLATFORM_WINDOWS
	void ScanControllersDirectInput();

	bool IsDirectInputDevice(const DirectInput::ControllerData& data) const { return data.driverType == DirectInput::ControllerData::DriverType::DIRECT_INPUT; }
	bool IsXInputDevice(const DirectInput::ControllerData& data) const { return data.driverType == DirectInput::ControllerData::DriverType::XINPUT; }
	bool IsScePadDevice(const DirectInput::ControllerData& data) const { return data.driverType == DirectInput::ControllerData::DriverType::SCEPAD; }
#endif

private:
	InputDevice* m_pDevices[(uint)InputDeviceIdentifier::_COUNT];
	bool m_IsExclusiveAccessReserved[(uint)InputDeviceIdentifier::_COUNT];

#ifdef BP_PLATFORM_WINDOWS
	DirectInput m_DirectInput;
	bool m_ShouldScanControllers;

	std::unordered_map<DirectInput::ControllerData, ControllerInputDevice*, DirectInput::ControllerData::Hasher> m_InUseControllerData;
#endif
};