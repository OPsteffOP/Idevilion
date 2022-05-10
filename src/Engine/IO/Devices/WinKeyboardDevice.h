// Folder: IO/Devices

#pragma once
#ifdef BP_PLATFORM_WINDOWS

#include "InputDevice.h"

class WinKeyboardDevice : public KeyboardInputDevice
{
public:
	virtual void Update() override;

private:
	uint ConvertInputAction(InputAction action) const;
};

#endif