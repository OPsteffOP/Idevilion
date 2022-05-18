// Folder: IO/Devices

#include "EnginePCH.h"
#include "InputDevice.h"

#include "InputManager.h"

InputDevice::InputDevice(InputDeviceIdentifier identifier)
	: m_Identifier(identifier)
	, m_Type(InputDeviceHelper::GetType(identifier))
{
	const uint controlsCount = InputDeviceHelper::GetControlsCount(m_Type);
	m_pActions = new Action[controlsCount];
	memset(m_pActions, 0, sizeof(Action) * controlsCount);
}

InputDevice::~InputDevice()
{
	SAFE_DELETE_ARRAY(m_pActions);
}

void InputDevice::SetState(uint action, float state)
{
	m_pActions[action].value = state;
}

void InputDevice::SetVector(uint actionX, uint actionY, int valueX, int valueY, int maxValue, int deadzone)
{
	const float magnitudeSquared = (float)(valueX * valueX + valueY * valueY);

	float newValueX = (float)valueX / (float)maxValue;
	float newValueY = (float)valueY / (float)maxValue;

	if (magnitudeSquared <= deadzone * deadzone)
	{
		newValueX = 0.f;
		newValueY = 0.f;
	}

	SetState(actionX, newValueX);
	SetState(actionY, newValueY);
}

float InputDevice::GetPreviousState(uint action) const
{
	if (InputManager::GetInstance()->IsExclusiveInput(m_Identifier))
		return 0.f;

	return m_pActions[action].previousValue;
}

float InputDevice::GetState(uint action) const
{
	if (InputManager::GetInstance()->IsExclusiveInput(m_Identifier))
		return 0.f;

	return m_pActions[action].value;
}

void InputDevice::InternalUpdate()
{
	// Commented this out for now because the controller needs to keep the state (all devices will now need to reset their own state if necessary)
	//const uint count = InputDeviceHelper::GetControlsCount(m_Type);
	//for (uint i = 0; i < count; ++i)
	//{
	//	m_pActions[i].previousValue = m_pActions[i].value;
	//	m_pActions[i].value = 0.f;
	//}

	Update();
}

float ExclusiveInputDevice::GetPreviousState(uint action) const
{
	return m_pDevice->m_pActions[action].previousValue;
}

float ExclusiveInputDevice::GetState(uint action) const
{
	return m_pDevice->m_pActions[action].value;
}

MouseInputDevice::MouseInputDevice(InputDeviceIdentifier identifier)
	: InputDevice(identifier)
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

KeyboardInputDevice::KeyboardInputDevice(InputDeviceIdentifier identifier)
	: InputDevice(identifier)
{}

bool KeyboardInputDevice::IsKeyDown(uint action) const
{
	return GetState(action) != 0.f;
}

bool KeyboardInputDevice::IsKeyUp(uint action) const
{
	return GetPreviousState(action) != 0.f && GetState(action) == 0.f;
}

ControllerInputDevice::ControllerInputDevice(InputDeviceIdentifier identifier)
	: InputDevice(identifier)
	, m_pBackend(nullptr)
{}

ControllerInputDevice::~ControllerInputDevice()
{
	SAFE_DELETE(m_pBackend);
}

void ControllerInputDevice::Update()
{
	if (m_pBackend != nullptr)
	{
		m_pBackend->Update();

		if (!m_pBackend->IsConnected())
		{
			SetBackend(nullptr);
			InputManager::GetInstance()->ForceScanControllersOnUpdate();
		}
	}
}

void ControllerInputDevice::SetBackend(ControllerBackend* pBackend)
{
	SAFE_DELETE(m_pBackend);
	m_pBackend = pBackend;

	const uint controlsCount = InputDeviceHelper::GetControlsCount(m_Type);
	memset(m_pActions, 0, sizeof(Action) * controlsCount);
}