// Folder: IO/Devices

#include "EnginePCH.h"
#include "InputDevice.h"

#include "InputManager.h"

InputDevice::InputDevice(DeviceType type)
	: m_Type(type)
{
	m_pActions = new Action[GetActionCountForDeviceType(m_Type)];
	memset(m_pActions, 0, sizeof(Action) * GetActionCountForDeviceType(m_Type));
}

InputDevice::~InputDevice()
{
	SAFE_DELETE_ARRAY(m_pActions);
}

void InputDevice::SetState(InputAction action, float state)
{
	m_pActions[(uint)action].value = state;
}

float InputDevice::GetPreviousState(InputAction action) const
{
	if (InputManager::GetInstance()->IsExclusiveInput(m_Type))
		return 0.f;

	return m_pActions[(uint)action].previousValue;
}

float InputDevice::GetState(InputAction action) const
{
	if (InputManager::GetInstance()->IsExclusiveInput(m_Type))
		return 0.f;

	return m_pActions[(uint)action].value;
}

void InputDevice::InternalUpdate()
{
	const uint count = GetActionCountForDeviceType(m_Type);
	for (uint i = 0; i < count; ++i)
	{
		m_pActions[i].previousValue = m_pActions[i].value;
		m_pActions[i].value = 0.f;
	}

	Update();
}

uint InputDevice::GetActionCountForDeviceType(DeviceType type) const
{
	switch (type)
	{
	case DeviceType::MOUSE:
		return (uint)InputAction::_COUNT_MOUSE;
	case DeviceType::KEYBOARD:
		return (uint)InputAction::_COUNT_KEYBOARD;
	}

	return 0;
}

float ExclusiveInputDevice::GetPreviousState(InputAction action) const
{
	return m_pDevice->m_pActions[(uint)action].previousValue;
}

float ExclusiveInputDevice::GetState(InputAction action) const
{
	return m_pDevice->m_pActions[(uint)action].value;
}

MouseInputDevice::MouseInputDevice()
	: InputDevice(DeviceType::MOUSE)
	, m_PreviousMousePosition()
	, m_MousePosition()
{}

const Point2f& MouseInputDevice::GetMousePosition() const
{
	return m_MousePosition;
}

Point2f MouseInputDevice::GetDeltaMousePosition() const
{
	return Point2f(m_MousePosition.x - m_PreviousMousePosition.x, m_MousePosition.y - m_PreviousMousePosition.y);
}

void MouseInputDevice::SetMousePosition(const Point2f& mousePosition)
{
	m_PreviousMousePosition = m_MousePosition;
	m_MousePosition = mousePosition;
}

KeyboardInputDevice::KeyboardInputDevice()
	: InputDevice(DeviceType::KEYBOARD)
{}

bool KeyboardInputDevice::IsKeyDown(InputAction action) const
{
	return GetState(action) != 0.f;
}

bool KeyboardInputDevice::IsKeyUp(InputAction action) const
{
	return GetPreviousState(action) != 0.f && GetState(action) == 0.f;
}