// Folder: Scene/Components/Core

#include "EnginePCH.h"
#include "DebugCameraComponent.h"

#include "CameraManager.h"
#include "DebugCamera.h"
#include "InputManager.h"

DebugCameraComponent::DebugCameraComponent()
	: DebugCameraComponent(new DebugCamera())
{}

DebugCameraComponent::DebugCameraComponent(Camera* pCamera)
	: CameraComponent(pCamera)
{
#ifdef BP_CONFIGURATION_DEBUG
	DEBUG_ASSERT(dynamic_cast<DebugCamera*>(m_pCamera) != nullptr, "DebugCamera instance is required when using a DebugCameraComponent");
#endif
}

void DebugCameraComponent::Update()
{
	DebugCamera* pDebugCamera = static_cast<DebugCamera*>(m_pCamera);

	KeyboardInputDevice* pKeyboardDevice = static_cast<KeyboardInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::KEYBOARD));

	if (pKeyboardDevice->IsKeyDown(InputAction::KEYBOARD_F2))
		CameraManager::GetInstance()->SetActiveCamera(pDebugCamera->GetMainCamera());
	if (pKeyboardDevice->IsKeyDown(InputAction::KEYBOARD_F3))
		CameraManager::GetInstance()->SetActiveCamera(pDebugCamera);

	CameraComponent::Update();
}

void DebugCameraComponent::Activated()
{
	DebugCamera* pDebugCamera = static_cast<DebugCamera*>(m_pCamera);
	pDebugCamera->SetMainCamera(CameraManager::GetInstance()->GetActiveCamera());
}