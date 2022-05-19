// Folder: ClientStates

#include "BloodPathPCH.h"
#include "ClientGame.h"

#include "WindowManager.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "GraphicsFactory.h"
#include "RetainedModeUISystem.h"

#include "FontBaking.h"

#include "StreamingScene.h"
#include "DebugCamera.h"

#include "CameraComponent.h"
#include "DebugCameraComponent.h"
#include "RenderComponent.h"
#include "AnimationRenderComponent.h"

ClientGame::ClientGame(const std::string& worldServerIP, uint worldServerPort, const NetworkUUID& sessionToken)
	: m_SessionToken(sessionToken)
	, m_pWorldServerClient(nullptr)
	, m_pScene(nullptr)
{
	UserVerifyStatus status = ConnectWorldServer(worldServerIP, worldServerPort);
	if (status != UserVerifyStatus::SUCCESSFUL)
	{
		// TODO: properly show status to user (NO_ACCESS, SERVER_DOWN)
		LOG_DEBUG("Failed to connect & verify on the world server, verify_status=%d", (uint)status);

		Shutdown();
		return;
	}
}

ClientGame::~ClientGame()
{
	ThreadPool::GetInstance()->Flush();
	SAFE_DELETE(m_pWorldServerClient);
	SAFE_DELETE(m_pScene);
}

void ClientGame::Initialize()
{
	Window* pMainWindow = WindowManager::GetInstance()->GetWindow(WindowIdentifier::MAIN_WINDOW);
	const uint mainWindowWidth = pMainWindow->GetWidth();
	const uint mainWindowHeight = pMainWindow->GetHeight();

	m_pScene = new StreamingScene(Paths::Data::DATA_DIR + Paths::Data::WORLDS_DIR + WORLD_FILE_NAME + Paths::Data::Extensions::WORLD_FILE);

	GameObject* pPlayerObject = new GameObject();
	DEBUG_ONLY(pPlayerObject->SetDebugName("Player Object"));
	pPlayerObject->AddComponent(new CameraComponent(new Camera(Point2f(mainWindowWidth / 2.f, mainWindowHeight / 2.f), Point2f((float)mainWindowWidth, (float)mainWindowHeight))));
	pPlayerObject->AddComponent(new DebugCameraComponent());
	//pPlayerObject->AddComponent(new RenderComponent(Paths::Data::DATA_DIR + "test.png", 16, 16));
	pPlayerObject->AddComponent(new AnimationRenderComponent(Paths::Data::DATA_DIR + "animation.png", 8, 2, 0, 8, 1.f, 32, 32));
	m_pScene->AddGameObject(pPlayerObject);
}

void ClientGame::Update()
{
	GameTime::Update();

	Window* pMainWindow = WindowManager::GetInstance()->GetWindow(WindowIdentifier::MAIN_WINDOW);
	Swapchain* pMainWindowSwapchain = pMainWindow->GetSwapchain();

	WindowManager::GetInstance()->SetCurrentWindow(WindowIdentifier::MAIN_WINDOW);
	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->ClearRenderTarget(pMainWindowSwapchain->GetDefaultRenderTarget(), Color{ 0.f, 0.f, 0.0f, 1.f });
	pRenderer->BindRenderTarget(pMainWindowSwapchain->GetDefaultRenderTarget());

	InputManager::GetInstance()->Update();
	CameraManager::GetInstance()->Update();
	pMainWindow->Update();

	m_pScene->UpdateChunkLoading(CameraManager::GetInstance()->GetActiveMainCamera());
	m_pScene->Update();

	RMUISystem::GetSystemForCurrentWindow()->Update();
}

void ClientGame::Render()
{
	Window* pMainWindow = WindowManager::GetInstance()->GetWindow(WindowIdentifier::MAIN_WINDOW);
	Swapchain* pMainWindowSwapchain = pMainWindow->GetSwapchain();

	WindowManager::GetInstance()->SetCurrentWindow(WindowIdentifier::MAIN_WINDOW);
	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->ClearRenderTarget(pMainWindowSwapchain->GetDefaultRenderTarget(), Color{ 0.f, 0.f, 0.3f, 1.f });
	pRenderer->BindRenderTarget(pMainWindowSwapchain->GetDefaultRenderTarget());

	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);

	if (CameraManager::GetInstance()->GetActiveCamera() != nullptr)
	{
		m_pScene->Render();
#ifdef DEV_DEBUG_BUILD
		m_pScene->DebugRender();
#endif

		CameraManager::GetInstance()->Render();
		RMUISystem::GetSystemForCurrentWindow()->Render();
	}

	pMainWindowSwapchain->Present();
}

UserVerifyStatus ClientGame::ConnectWorldServer(const std::string& ip, uint port)
{
	m_pWorldServerClient = NFactory::CreateClient();
	//m_pWorldServerClient->SetConnectionDiedCallback(); // TODO
	m_pWorldServerClient->Connect(ip, port);

	if (!m_pWorldServerClient->IsConnected())
		return UserVerifyStatus::SERVER_DOWN;

	NPacket verifyPacket;
	verifyPacket.identifier = (NPacketIdentifier)GamePacket::AUTHENTICATE;
	verifyPacket.data.WriteNetworkUUID(m_SessionToken);

	m_pWorldServerClient->SendPacket(verifyPacket);

	NPacket receivedPacket = m_pWorldServerClient->ReadPacket();
	if ((GamePacket)receivedPacket.identifier != GamePacket::AUTHENTICATE)
		return UserVerifyStatus::FAILURE;

	UserVerifyStatus status = (UserVerifyStatus)receivedPacket.data.ReadUChar();
	if (status != UserVerifyStatus::SUCCESSFUL)
		return status;

	uint8_t privilegeLevel = receivedPacket.data.ReadUInt8(); // TODO: do something with the received privilegeLevel
	UNREFERENCED_PARAMETER(privilegeLevel);

	return status;
}