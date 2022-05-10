// Folder: IO/Devices

#pragma once
#include "InputDevice.h"

class InputManager
{
public:
	static InputManager* GetInstance();
	~InputManager();

	void Update();

	InputDevice* GetDevice(DeviceType type) const;
	void SetDevice(DeviceType type, InputDevice* pDevice);

	bool IsExclusiveInput(DeviceType type) const;
	ExclusiveInputDevice* ClaimExclusiveInput(DeviceType type);
	void ReleaseExclusiveInput(ExclusiveInputDevice* pDevice);

private:
	InputManager();
	
	void FindDevices();

private:
	InputDevice* m_pDevices[(uint)DeviceType::_COUNT];
	bool m_IsExclusiveAccessReserved[(uint)DeviceType::_COUNT];
};