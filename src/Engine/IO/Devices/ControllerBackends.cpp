// Folder: IO/Devices

#include "EnginePCH.h"
#include "ControllerBackends.h"

#include "InputDevice.h"

ControllerBackend::ControllerBackend(ControllerInputDevice* pDevice, ControllerType type)
	: m_pDevice(pDevice)
	, m_Type(type)
{}

#ifdef BP_PLATFORM_WINDOWS
std::queue<uint> XboxControllerBackend::m_AvailableXInputIndexes = std::queue<uint>(std::initializer_list<uint>{ 0, 1, 2, 3 });

XboxControllerBackend::XboxControllerBackend(ControllerInputDevice* pDevice)
	: ControllerBackend(pDevice, ControllerType::XBOX)
	, m_XInputIndex(-1)
	, m_LastPacketNumber(0)
{
	if (m_AvailableXInputIndexes.empty())
		return;

	const uint indexesCount = (uint)m_AvailableXInputIndexes.size();
	uint i = 0;
	while (m_XInputIndex == -1 && ++i <= indexesCount)
	{
		const uint XInputIndex = m_AvailableXInputIndexes.front();
		m_AvailableXInputIndexes.pop();

		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		DWORD result = XInputGetState(XInputIndex, &state);

		if (result == ERROR_DEVICE_NOT_CONNECTED)
		{
			m_AvailableXInputIndexes.push(XInputIndex);
			continue;
		}

		m_XInputIndex = XInputIndex;
		break;
	}
}

XboxControllerBackend::~XboxControllerBackend()
{
	if (m_XInputIndex != -1)
		m_AvailableXInputIndexes.push(m_XInputIndex);
}

void XboxControllerBackend::Update()
{
	if (m_XInputIndex == -1)
		return;

	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	DWORD result = XInputGetState(m_XInputIndex, &state);

	m_pDevice->SetState((uint)ControllerControl::CONTROLLER_CONNECTED, (float)(result != ERROR_DEVICE_NOT_CONNECTED));

	if (result != ERROR_SUCCESS)
		return;

	if (state.dwPacketNumber != m_LastPacketNumber)
	{
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_DPAD_UP, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_DPAD_DOWN, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_DPAD_LEFT, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_DPAD_RIGHT, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_START, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_START));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_BACK, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_LEFT_THUMB_PRESS, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_RIGHT_THUMB_PRESS, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_LEFT_SHOULDER, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_RIGHT_SHOULDER, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_A, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_A));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_B, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_B));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_X, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_X));
		m_pDevice->SetState((uint)ControllerControl::CONTROLLER_Y, (bool)(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y));

		m_pDevice->SetVector((uint)ControllerControl::CONTROLLER_LEFT_TRIGGER, (uint)ControllerControl::CONTROLLER_RIGHT_TRIGGER, state.Gamepad.bLeftTrigger, state.Gamepad.bRightTrigger, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
		m_pDevice->SetVector((uint)ControllerControl::CONTROLLER_LEFT_THUMB_X, (uint)ControllerControl::CONTROLLER_LEFT_THUMB_Y, state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, 32767, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		m_pDevice->SetVector((uint)ControllerControl::CONTROLLER_RIGHT_THUMB_X, (uint)ControllerControl::CONTROLLER_RIGHT_THUMB_Y, state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, 32767, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

		m_LastPacketNumber = state.dwPacketNumber;
	}
}

bool XboxControllerBackend::IsConnected() const
{
	return (bool)m_pDevice->GetState((uint)ControllerControl::CONTROLLER_CONNECTED);
}
#endif