// Folder: IO/Devices

#pragma once
#ifdef BP_PLATFORM_WINDOWS

#include "InputDevice.h"

class WinMouseDevice : public MouseInputDevice
{
public:
	virtual void Update() override;

	virtual bool IsCursorVisible() const override;
	virtual void SetCursorVisible(bool isVisible) override;

private:
	bool m_IsCursorVisible;
};

#endif