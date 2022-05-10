// Folder: Scene/Components/Core

#pragma once
#include "Component.h"
#include "Camera.h"

class CameraComponent : public Component
{
public:
	CameraComponent();
	CameraComponent(Camera* pCamera);

	virtual ~CameraComponent() override;

	virtual void Update() override;

	virtual void Activated() override;
	virtual void Deactivated() override;

protected:
	Camera* m_pCamera;
};