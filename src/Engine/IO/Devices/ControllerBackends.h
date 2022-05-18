// Folder: IO/Devices

#pragma once

class ControllerInputDevice;

enum class ControllerType
{
	XBOX
};

class ControllerBackend
{
public:
	explicit ControllerBackend(ControllerInputDevice* pDevice, ControllerType type);
	virtual ~ControllerBackend() = default;

	virtual void Update() = 0;

	virtual bool IsConnected() const = 0;

protected:
	ControllerInputDevice* m_pDevice;
	ControllerType m_Type;
};

#ifdef BP_PLATFORM_WINDOWS
class XboxControllerBackend : public ControllerBackend
{
public:
	explicit XboxControllerBackend(ControllerInputDevice* pDevice);
	virtual ~XboxControllerBackend() override;

	virtual void Update() override;

	virtual bool IsConnected() const override;

private:
	static std::queue<uint> m_AvailableXInputIndexes;

	int m_XInputIndex;
	DWORD m_LastPacketNumber;
};
#endif