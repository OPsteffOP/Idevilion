// Folder: IO/Devices

#pragma once
#ifdef BP_PLATFORM_WINDOWS

#include "InputDevice.h"

class WinKeyboardDevice : public KeyboardInputDevice
{
public:
	explicit WinKeyboardDevice(InputDeviceIdentifier identifier);

	virtual void Update() override;

private:
	uint ConvertInputAction(KeyboardControl action) const;
};

#endif