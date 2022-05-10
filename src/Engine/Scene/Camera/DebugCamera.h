// Folder: Scene/Camera

#pragma once
#include "Camera.h"

class Buffer;
class ShaderState;
class InputLayout;

class DebugCamera : public Camera
{
public:
	DebugCamera();

	virtual void Update() override;
	virtual void Render() override;

	void SetMainCamera(Camera* pMainCamera);
	Camera* GetMainCamera() const { return m_pMainCamera; }

	virtual bool IsDebugCamera() const override { return true; }

private:
	void UpdateVertexBuffer();

private:
	static constexpr uint VERTEX_COUNT = 8;
	static Buffer* m_pVertexBuffer;
	static ShaderState* m_pVertexShader;
	static ShaderState* m_pPixelShader;
	static InputLayout* m_pInputLayout;

	Camera* m_pMainCamera;
	float m_DebugCameraScaleFactor;
	float m_MainCameraScale;
};