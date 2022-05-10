// Folder: Scene/Components/Core

#pragma once
#include "CameraComponent.h"
#include "Camera.h"

class DebugCameraComponent : public CameraComponent
{
public:
	DebugCameraComponent();
	DebugCameraComponent(Camera* pCamera);

	virtual void Update() override;

	virtual void Activated() override;
};