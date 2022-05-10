// Folder: Scene/Camera

#pragma once

class Camera;

class CameraManager
{
public:
	static CameraManager* GetInstance();

	void Update();
	void Render();
	
	Camera* GetActiveCamera() const;
	void SetActiveCamera(Camera* pCamera);

	Camera* GetActiveMainCamera() const;

private:
	CameraManager();

private:
	Camera* m_pActiveCamera;
};