// Folder: Scene/Components/Core

#include "EnginePCH.h"
#include "CameraComponent.h"

#include "CameraManager.h"
#include "InputManager.h"
#include "GameObject.h"

CameraComponent::CameraComponent()
	: CameraComponent(new Camera())
{}

CameraComponent::CameraComponent(Camera* pCamera)
	: m_pCamera(pCamera)
{}

CameraComponent::~CameraComponent()
{
	SAFE_DELETE(m_pCamera);
}

void CameraComponent::Update()
{
	if (CameraManager::GetInstance()->GetActiveCamera() != m_pCamera)
		return;

	KeyboardInputDevice* pKeyboardDevice = static_cast<KeyboardInputDevice*>(InputManager::GetInstance()->GetDevice(InputDeviceIdentifier::KEYBOARD));
	MouseInputDevice* pMouseDevice = static_cast<MouseInputDevice*>(InputManager::GetInstance()->GetDevice(InputDeviceIdentifier::MOUSE));

	if (pKeyboardDevice->IsKeyDown((uint)KeyboardControl::KEYBOARD_RIGHT_ARROW))
	{
		m_pGameObject->Move(0.25f, 0.f);
		CameraManager::GetInstance()->GetActiveMainCamera()->SetCenterPosition(m_pGameObject->GetPosition()); // TODO: camera should be moved with the same delta movement instead of teleported to the exact player position
	}
	if (pKeyboardDevice->IsKeyDown((uint)KeyboardControl::KEYBOARD_LEFT_ARROW))
	{
		m_pGameObject->Move(-0.25f, 0.f);
		CameraManager::GetInstance()->GetActiveMainCamera()->SetCenterPosition(m_pGameObject->GetPosition()); // TODO: camera should be moved with the same delta movement instead of teleported to the exact player position
	}
	if (pKeyboardDevice->IsKeyDown((uint)KeyboardControl::KEYBOARD_UP_ARROW))
	{
		m_pGameObject->Move(0.f, 0.25f);
		CameraManager::GetInstance()->GetActiveMainCamera()->SetCenterPosition(m_pGameObject->GetPosition()); // TODO: camera should be moved with the same delta movement instead of teleported to the exact player position
	}
	if (pKeyboardDevice->IsKeyDown((uint)KeyboardControl::KEYBOARD_DOWN_ARROW))
	{
		m_pGameObject->Move(0.f, -0.25f);
		CameraManager::GetInstance()->GetActiveMainCamera()->SetCenterPosition(m_pGameObject->GetPosition()); // TODO: camera should be moved with the same delta movement instead of teleported to the exact player position
	}

	// TODO: TEMP - TESTING
	ControllerInputDevice* pControllerDevice1 = static_cast<ControllerInputDevice*>(InputManager::GetInstance()->GetDevice(InputDeviceIdentifier::CONTROLLER_1));
	m_pGameObject->Move(0.25f * pControllerDevice1->GetState((uint)ControllerControl::CONTROLLER_LEFT_THUMB_X), 0.25f * pControllerDevice1->GetState((uint)ControllerControl::CONTROLLER_LEFT_THUMB_Y));
	ControllerInputDevice* pControllerDevice2 = static_cast<ControllerInputDevice*>(InputManager::GetInstance()->GetDevice(InputDeviceIdentifier::CONTROLLER_2));
	m_pGameObject->Move(0.25f * pControllerDevice2->GetState((uint)ControllerControl::CONTROLLER_LEFT_THUMB_X), 0.25f * pControllerDevice2->GetState((uint)ControllerControl::CONTROLLER_LEFT_THUMB_Y));
	ControllerInputDevice* pControllerDevice3 = static_cast<ControllerInputDevice*>(InputManager::GetInstance()->GetDevice(InputDeviceIdentifier::CONTROLLER_3));
	m_pGameObject->Move(0.25f * pControllerDevice3->GetState((uint)ControllerControl::CONTROLLER_LEFT_THUMB_X), 0.25f * pControllerDevice3->GetState((uint)ControllerControl::CONTROLLER_LEFT_THUMB_Y));
	ControllerInputDevice* pControllerDevice4 = static_cast<ControllerInputDevice*>(InputManager::GetInstance()->GetDevice(InputDeviceIdentifier::CONTROLLER_4));
	m_pGameObject->Move(0.25f * pControllerDevice4->GetState((uint)ControllerControl::CONTROLLER_LEFT_THUMB_X), 0.25f * pControllerDevice4->GetState((uint)ControllerControl::CONTROLLER_LEFT_THUMB_Y));

	CameraManager::GetInstance()->GetActiveMainCamera()->SetCenterPosition(m_pGameObject->GetPosition());
	// TODO: TEMP - TESTING

	if (pMouseDevice->GetState((uint)MouseControl::MOUSE_SCROLL))
	{
		const float scrollAmount = -pMouseDevice->GetState((uint)MouseControl::MOUSE_SCROLL);
		if (std::fabsf(scrollAmount) >= FLT_EPSILON)
		{
			const float previousScale = CameraManager::GetInstance()->GetActiveMainCamera()->GetScale();
			const float scrollMultiplier = 0.1f;
			const float deltaScale = scrollAmount * scrollMultiplier;
			const float newScale = previousScale + deltaScale;

			CameraManager::GetInstance()->GetActiveMainCamera()->SetScale(newScale);
		}
	}
}

void CameraComponent::Activated()
{
	// TODO: probably shouldn't always immediately activate the camera when the GameObject is enabled
	m_pGameObject->SetPosition(m_pCamera->GetCenterPosition());
	CameraManager::GetInstance()->SetActiveCamera(m_pCamera);
}

void CameraComponent::Deactivated()
{
	if (CameraManager::GetInstance()->GetActiveCamera() == m_pCamera)
		CameraManager::GetInstance()->SetActiveCamera(nullptr);
}