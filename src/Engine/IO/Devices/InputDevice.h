// Folder: IO/Devices

#pragma once
#include "InputEnums.h"
#include "ControllerBackends.h"

class InputDevice
{
	friend class InputManager;
	friend class ExclusiveInputDevice;

public:
	explicit InputDevice(InputDeviceIdentifier identifier);
	virtual ~InputDevice();

	virtual void Update() = 0;

	void SetState(uint action, float state);
	void SetVector(uint actionX, uint actionY, int valueX, int valueY, int maxValue, int deadzone);
	float GetPreviousState(uint action) const;
	float GetState(uint action) const;

	InputDeviceIdentifier GetIdentifier() const { return m_Identifier; }
	InputDeviceType GetType() const { return m_Type; }

protected:
	const InputDeviceIdentifier m_Identifier;
	const InputDeviceType m_Type;

	struct Action
	{
		float value;
		float previousValue;
	};
	Action* m_pActions;

private:
	void InternalUpdate();
};

class ExclusiveInputDevice
{
	friend class InputManager;

public:
	float GetPreviousState(uint action) const;
	float GetState(uint action) const;

private:
	ExclusiveInputDevice() = default;

private:
	InputDevice* m_pDevice;
};

class MouseInputDevice : public InputDevice
{
public:
	explicit MouseInputDevice(InputDeviceIdentifier identifier);

	const Point2f& GetMousePosition() const;
	Point2f GetDeltaMousePosition() const;
	void SetMousePosition(const Point2f& mousePosition);

	virtual bool IsCursorVisible() const = 0;
	virtual void SetCursorVisible(bool isVisible) = 0;

protected:
	Point2f m_PreviousMousePosition;
	Point2f m_MousePosition;
};

class KeyboardInputDevice : public InputDevice
{
public:
	explicit KeyboardInputDevice(InputDeviceIdentifier identifier);

	bool IsKeyDown(uint action) const;
	bool IsKeyUp(uint action) const;
};

class ControllerInputDevice : public InputDevice
{
	friend class ControllerBackend;

public:
	explicit ControllerInputDevice(InputDeviceIdentifier identifier);
	virtual ~ControllerInputDevice() override;

	virtual void Update() override;

	ControllerBackend* GetBackend() const { return m_pBackend; }
	void SetBackend(ControllerBackend* pBackend);

protected:
	ControllerBackend* m_pBackend;
};