// Folder: Scene/Camera

#include "EnginePCH.h"
#include "CameraManager.h"
#include "Camera.h"
#include "DebugCamera.h"

CameraManager* CameraManager::GetInstance()
{
	static CameraManager* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new CameraManager();
	}

	return pInstance;
}

CameraManager::CameraManager()
	: m_pActiveCamera(nullptr)
{}

void CameraManager::Update()
{
	if (m_pActiveCamera != nullptr)
		m_pActiveCamera->Update();
}

void CameraManager::Render()
{
	if (m_pActiveCamera != nullptr)
		m_pActiveCamera->Render();
}

Camera* CameraManager::GetActiveCamera() const
{
	return m_pActiveCamera;
}

void CameraManager::SetActiveCamera(Camera* pCamera)
{
	m_pActiveCamera = pCamera;

	if (m_pActiveCamera != nullptr)
		m_pActiveCamera->UpdateActiveCameraBuffer();
}

Camera* CameraManager::GetActiveMainCamera() const
{
	if (m_pActiveCamera != nullptr && m_pActiveCamera->IsDebugCamera())
		return static_cast<DebugCamera*>(m_pActiveCamera)->GetMainCamera();

	return m_pActiveCamera;
}