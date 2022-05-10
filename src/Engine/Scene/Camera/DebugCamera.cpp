// Folder: Scene/Camera

#include "EnginePCH.h"
#include "DebugCamera.h"

#include "GraphicsFactory.h"
#include "Renderer.h"
#include "CameraManager.h"

Buffer* DebugCamera::m_pVertexBuffer = nullptr;
ShaderState* DebugCamera::m_pVertexShader = nullptr;
ShaderState* DebugCamera::m_pPixelShader = nullptr;
InputLayout* DebugCamera::m_pInputLayout = nullptr;

DebugCamera::DebugCamera()
	: m_pMainCamera(nullptr)
	, m_DebugCameraScaleFactor(2.f)
	, m_MainCameraScale(FLT_MAX)
{
	if (m_pVertexBuffer == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		m_pVertexBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(Point2f), VERTEX_COUNT, BufferUsage::DYNAMIC,
			ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::VERTEX_BUFFER);

		ALLOW_NEXT_ALLOCATION_LEAK();
		m_pVertexShader = GraphicsFactory::CreateShaderState(ShaderType::VERTEX_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "DefaultLineVS.hlsl");
		ALLOW_NEXT_ALLOCATION_LEAK();
		m_pPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "DefaultLinePS.hlsl");

		std::vector<InputElement> inputElements;
		inputElements.reserve(1);
		inputElements.push_back(InputElement{ "POSITION", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
		ALLOW_NEXT_ALLOCATION_LEAK();
		m_pInputLayout = GraphicsFactory::CreateInputLayout(m_pVertexShader, inputElements);

		UpdateVertexBuffer();
	}
}

void DebugCamera::Update()
{
	if (m_pMainCamera == nullptr)
		return;

	if (m_CenterPosition != m_pMainCamera->m_CenterPosition)
	{
		m_CenterPosition = m_pMainCamera->m_CenterPosition;
		m_Size = m_pMainCamera->m_Size;
		m_DirtyFlag = true;
	}

	if (m_MainCameraScale != m_pMainCamera->m_Scale)
	{
		m_MainCameraScale = m_pMainCamera->m_Scale;
		m_Scale = m_MainCameraScale * m_DebugCameraScaleFactor;
		m_DirtyFlag = true;
	}

	if (std::abs((m_MainCameraScale * m_DebugCameraScaleFactor) - m_Scale) > FLT_EPSILON)
	{
		m_DebugCameraScaleFactor = m_Scale / m_MainCameraScale;
		UpdateVertexBuffer();
	}

	Camera::Update();
}

void DebugCamera::Render()
{
	if (m_pMainCamera == nullptr)
		return;

	Camera::Render();

	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::LINES);

	pRenderer->BindVertexBuffer(m_pVertexBuffer);
	pRenderer->BindShaderState(m_pVertexShader);
	pRenderer->BindShaderState(m_pPixelShader);
	pRenderer->BindInputLayout(m_pInputLayout);

	pRenderer->Draw(VERTEX_COUNT);

	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);
}

void DebugCamera::UpdateVertexBuffer()
{
	Point2f vertices[VERTEX_COUNT]
	{
		Point2f{ -1.f / m_DebugCameraScaleFactor,  1.f / m_DebugCameraScaleFactor }, Point2f{  1.f / m_DebugCameraScaleFactor,  1.f / m_DebugCameraScaleFactor },
		Point2f{  1.f / m_DebugCameraScaleFactor,  1.f / m_DebugCameraScaleFactor }, Point2f{  1.f / m_DebugCameraScaleFactor, -1.f / m_DebugCameraScaleFactor },
		Point2f{  1.f / m_DebugCameraScaleFactor, -1.f / m_DebugCameraScaleFactor }, Point2f{ -1.f / m_DebugCameraScaleFactor, -1.f / m_DebugCameraScaleFactor },
		Point2f{ -1.f / m_DebugCameraScaleFactor, -1.f / m_DebugCameraScaleFactor }, Point2f{ -1.f / m_DebugCameraScaleFactor,  1.f / m_DebugCameraScaleFactor }
	};
	m_pVertexBuffer->UpdateData(vertices);
}

void DebugCamera::SetMainCamera(Camera* pMainCamera)
{
	m_pMainCamera = pMainCamera;
}