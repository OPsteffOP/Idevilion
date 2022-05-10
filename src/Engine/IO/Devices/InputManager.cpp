// Folder: IO/Devices

#include "EnginePCH.h"
#include "InputManager.h"

#include "WinMouseDevice.h"
#include "WinKeyboardDevice.h"

InputManager* InputManager::GetInstance()
{
	static InputManager* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new InputManager();
	}

	return pInstance;
}

InputManager::InputManager()
{
	std::fill(std::begin(m_pDevices), std::end(m_pDevices), nullptr);
	std::fill(std::begin(m_IsExclusiveAccessReserved), std::end(m_IsExclusiveAccessReserved), false);
}

InputManager::~InputManager()
{
	for (uint i = 0; i < (uint)DeviceType::_COUNT; ++i)
	{
		SAFE_DELETE(m_pDevices[i]);
	}
}

void InputManager::Update()
{
	FindDevices();

	for (uint i = 0; i < (uint)DeviceType::_COUNT; ++i)
	{
		InputDevice* pDevice = m_pDevices[i];
		if (pDevice != nullptr)
			pDevice->InternalUpdate();
	}
}

InputDevice* InputManager::GetDevice(DeviceType type) const
{
	return m_pDevices[(uint)type];
}

void InputManager::SetDevice(DeviceType type, InputDevice* pDevice)
{
	if (type != pDevice->GetType())
	{
		LOG_ERROR("Trying to set a device with a different type - expected_type=%d, provided_type=%d", (uint)type, (uint)pDevice->GetType());
		return;
	}

	if (m_pDevices[(uint)type] != nullptr)
	{
		SAFE_DELETE(m_pDevices[(uint)type]);
		LOG_DEBUG("Setting device for type that already exists, deleting existing device and overwritting with new one");
	}

	m_pDevices[(uint)type] = pDevice;
}

bool InputManager::IsExclusiveInput(DeviceType type) const
{
	return m_IsExclusiveAccessReserved[(uint)type];
}

ExclusiveInputDevice* InputManager::ClaimExclusiveInput(DeviceType type)
{
	if (IsExclusiveInput(type))
		return nullptr;

	m_IsExclusiveAccessReserved[(uint)type] = true;

	ExclusiveInputDevice* pDevice = new ExclusiveInputDevice();
	pDevice->m_pDevice = GetDevice(type);

	return pDevice;
}

void InputManager::ReleaseExclusiveInput(ExclusiveInputDevice* pDevice)
{
	m_IsExclusiveAccessReserved[(uint)pDevice->m_pDevice->GetType()] = false;
	SAFE_DELETE(pDevice);
}

void InputManager::FindDevices()
{
	if (m_pDevices[(uint)DeviceType::MOUSE] == nullptr)
	{
#ifdef BP_PLATFORM_WINDOWS
		SetDevice(DeviceType::MOUSE, new WinMouseDevice());
#endif
	}

	if (m_pDevices[(uint)DeviceType::KEYBOARD] == nullptr)
	{
#ifdef BP_PLATFORM_WINDOWS
		SetDevice(DeviceType::KEYBOARD, new WinKeyboardDevice());
#endif
	}
}