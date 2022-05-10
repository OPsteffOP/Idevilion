// Folder: Window

#pragma once

class Swapchain;

class Window
{
public:
	typedef std::function<void(Window* pWindow)> WindowEventCallback;

public:
	Window(const std::string& title, uint width, uint height, void* pUserData);

	virtual ~Window();

	virtual void Update() {}

	virtual void Close() {}

	uint GetWidth() const { return m_Width; }
	uint GetHeight() const { return m_Height; }
	WindowHandle GetWindowHandle() const { return m_pWindowHandle; }
	bool IsClosed() const { return m_pWindowHandle == nullptr; }
	virtual bool IsFocused() const = 0;
	virtual Point2f GetLocalMousePosition() const = 0;

	void SetSwapchain(Swapchain* pSwapchain) { m_pSwapchain = pSwapchain; }
	Swapchain* GetSwapchain() const { return m_pSwapchain; }

	void SetCloseCallback(WindowEventCallback&& onCloseCallback) { m_OnCloseCallback = std::move(onCloseCallback); }
	void SetUnfocusCallback(WindowEventCallback&& onUnfocusCallback) { m_OnUnfocusCallback = std::move(onUnfocusCallback); }
	void SetFocusCallback(WindowEventCallback&& onFocusCallback) { m_OnFocusCallback = std::move(onFocusCallback); }

protected:
	std::string m_Title;
	uint m_Width;
	uint m_Height;

	WindowHandle m_pWindowHandle;

	WindowEventCallback m_OnCloseCallback;
	WindowEventCallback m_OnUnfocusCallback;
	WindowEventCallback m_OnFocusCallback;

	Swapchain* m_pSwapchain;
};