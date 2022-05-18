// Folder: IO/Devices

#include "EnginePCH.h"
#include "InputManager.h"

#include "WinMouseDevice.h"
#include "WinKeyboardDevice.h"

#ifdef BP_PLATFORM_WINDOWS
#include <Dbt.h>
#endif

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

	CreateDevices();
}

InputManager::~InputManager()
{
	for (uint i = 0; i < (uint)InputDeviceIdentifier::_COUNT; ++i)
	{
		SAFE_DELETE(m_pDevices[i]);
	}
}

void InputManager::Update()
{
	for (uint i = 0; i < (uint)InputDeviceIdentifier::_COUNT; ++i)
	{
		InputDevice* pDevice = m_pDevices[i];
		if (pDevice != nullptr)
			pDevice->InternalUpdate();
	}

#ifdef BP_PLATFORM_WINDOWS
	if (m_ShouldScanControllers)
	{
		ScanControllersDirectInput();
		m_ShouldScanControllers = false;
	}
#endif
}

InputDevice* InputManager::GetDevice(InputDeviceIdentifier identifier) const
{
	return m_pDevices[(uint)identifier];
}

void InputManager::SetDevice(InputDevice* pDevice)
{
	if (m_pDevices[(uint)pDevice->GetIdentifier()] != nullptr)
	{
		SAFE_DELETE(m_pDevices[(uint)pDevice->GetIdentifier()]);
		LOG_DEBUG("Setting device for type that already exists, deleting existing device and overwritting with new one");
	}

	m_pDevices[(uint)pDevice->GetIdentifier()] = pDevice;
}

bool InputManager::IsExclusiveInput(InputDeviceIdentifier identifier) const
{
	return m_IsExclusiveAccessReserved[(uint)identifier];
}

ExclusiveInputDevice* InputManager::ClaimExclusiveInput(InputDeviceIdentifier identifier)
{
	if (IsExclusiveInput(identifier))
		return nullptr;

	m_IsExclusiveAccessReserved[(uint)identifier] = true;

	ExclusiveInputDevice* pDevice = new ExclusiveInputDevice();
	pDevice->m_pDevice = GetDevice(identifier);

	return pDevice;
}

void InputManager::ReleaseExclusiveInput(ExclusiveInputDevice* pDevice)
{
	m_IsExclusiveAccessReserved[(uint)pDevice->m_pDevice->GetIdentifier()] = false;
	SAFE_DELETE(pDevice);
}

void InputManager::CreateDevices()
{
	if (m_pDevices[(uint)InputDeviceIdentifier::MOUSE] == nullptr)
	{
#ifdef BP_PLATFORM_WINDOWS
		SetDevice(new WinMouseDevice(InputDeviceIdentifier::MOUSE));
#endif
	}

	if (m_pDevices[(uint)InputDeviceIdentifier::KEYBOARD] == nullptr)
	{
#ifdef BP_PLATFORM_WINDOWS
		SetDevice(new WinKeyboardDevice(InputDeviceIdentifier::KEYBOARD));
#endif
	}

	for (uint i = (uint)InputDeviceIdentifier::CONTROLLER_1; i <= (uint)InputDeviceIdentifier::CONTROLLER_4; ++i)
	{
		if (m_pDevices[i] == nullptr)
			SetDevice(new ControllerInputDevice((InputDeviceIdentifier)i));
	}
}

#ifdef BP_PLATFORM_WINDOWS
void InputManager::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDBLCLK:
	{
		MouseInputDevice* pMouseDevice = static_cast<MouseInputDevice*>(InputManager::GetInstance()->GetDevice(InputDeviceIdentifier::MOUSE));
		pMouseDevice->SetState((uint)MouseControl::MOUSE_LEFT_DOUBLE_CLICK, 1.f);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		MouseInputDevice* pMouseDevice = static_cast<MouseInputDevice*>(InputManager::GetInstance()->GetDevice(InputDeviceIdentifier::MOUSE));
		pMouseDevice->SetState((uint)MouseControl::MOUSE_SCROLL, (float)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		break;
	}

	case WM_CREATE:
	case WM_DEVICECHANGE:
	{
		InputManager::GetInstance()->ForceScanControllersOnUpdate();
		break;
	}
	}

	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
}

void InputManager::ScanControllersDirectInput()
{
	for (auto it = m_InUseControllerData.begin(); it != m_InUseControllerData.end();)
	{
		if (it->second->GetBackend() == nullptr)
			it = m_InUseControllerData.erase(it);
		else
			++it;
	}

	const std::unordered_set<DirectInput::ControllerData, DirectInput::ControllerData::Hasher>& devicesData = m_DirectInput.EnumerateDevices();

	for (uint i = (uint)InputDeviceIdentifier::CONTROLLER_1; i <= (uint)InputDeviceIdentifier::CONTROLLER_4; ++i)
	{
		if (m_pDevices[i] == nullptr)
			continue;

		ControllerInputDevice* pDevice = static_cast<ControllerInputDevice*>(m_pDevices[i]);
		if (pDevice->GetBackend() == nullptr)
		{
			for (const DirectInput::ControllerData& data : devicesData)
			{
				if (m_InUseControllerData.find(data) != m_InUseControllerData.end())
					continue;

				if (IsDirectInputDevice(data))
				{
					// TODO: direct input devices not supported yet
					continue;
				}

				if (IsXInputDevice(data))
					pDevice->SetBackend(new XboxControllerBackend(pDevice));
				else if (IsScePadDevice(data))
					continue; //pDevice->SetBackend(/* TODO: scepad backend */); // TODO: add once access to devnet

				m_InUseControllerData.emplace(data, pDevice);
				break;
			}
		}
	}
}
#endif