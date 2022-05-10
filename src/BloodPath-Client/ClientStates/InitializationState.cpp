// Folder: ClientStates

#include "BloodPathPCH.h"
#include "InitializationState.h"

#include "WindowManager.h"
#include "GraphicsFactory.h"

#include "FontBaking.h"

namespace _Private
{
	constexpr static const char* MAIN_WINDOW_TITLE = "BloodPath Client";
	constexpr static const uint MAIN_WINDOW_WIDTH = 1280;
	constexpr static const uint MAIN_WINDOW_HEIGHT = 720;
}

bool InitializationState::Initialize(void* pWindowUserData)
{
	// Initialize renderer
	Renderer* pRenderer = GraphicsFactory::CreateRenderer(RenderAPI::PlatformDefault);
	RendererFlags initFlags = RendererFlags::NONE;
#ifdef BP_CONFIGURATION_DEBUG
	initFlags |= RendererFlags::ENABLE_DEBUGGING;
#endif
	pRenderer->Initialize(initFlags);
	Renderer::SetRenderer(pRenderer);

	// Bake if necessary
	if (CommandLine::IsFlagSet("--bake") || CommandLine::IsFlagSet("--bake-and-run"))
	{
		Baking::BakeFontSDFBitmap("Arial"); // TODO: don't hardcode which font to bake
		if (CommandLine::IsFlagSet("--bake"))
			return false;
	}

	// Window and swapchain creation
	Window* pMainWindow = WindowManager::GetInstance()->CreateWindow(WindowIdentifier::MAIN_WINDOW, _Private::MAIN_WINDOW_TITLE, _Private::MAIN_WINDOW_WIDTH, _Private::MAIN_WINDOW_HEIGHT, pWindowUserData);
	WindowManager::GetInstance()->SetCurrentWindow(WindowIdentifier::MAIN_WINDOW);

	Swapchain* pSwapchain = GraphicsFactory::CreateSwapchain(_Private::MAIN_WINDOW_WIDTH, _Private::MAIN_WINDOW_HEIGHT, pMainWindow->GetWindowHandle());
	pMainWindow->SetSwapchain(pSwapchain);

	return true;
}