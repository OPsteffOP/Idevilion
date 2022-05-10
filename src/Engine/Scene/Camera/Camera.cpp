// Folder: Scene/Camera

#include "EnginePCH.h"
#include "Camera.h"

#include "Renderer.h"
#include "GraphicsFactory.h"
#include "Buffer.h"

Buffer* Camera::m_pConstantBuffer = nullptr;

struct CameraBufferElement
{
	float width;
	float height;
	float scale;
	float positionX;
	float positionY;
	float _reserved[3];
};

Camera::Camera()
	: Camera(Point2f(), Point2f())
{}

Camera::Camera(const Point2f& centerPosition, const Point2f& size, float minScale, float maxScale)
	: m_DirtyFlag(true)
	, m_CenterPosition(centerPosition)
	, m_Size(size)
	, m_Scale(1.f)
	, m_MinScale(minScale)
	, m_MaxScale(maxScale)
{
	if (m_pConstantBuffer == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		m_pConstantBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(CameraBufferElement), 1, BufferUsage::DYNAMIC,
			ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::CONSTANT_BUFFER);

		Renderer* pRenderer = Renderer::GetRenderer();
		pRenderer->BindConstantBuffer_Internal(ShaderType::VERTEX_SHADER, { m_pConstantBuffer }, (uint)ConstantBufferIndex::RESERVED_CAMERA_DATA);
		pRenderer->BindConstantBuffer_Internal(ShaderType::PIXEL_SHADER, { m_pConstantBuffer }, (uint)ConstantBufferIndex::RESERVED_CAMERA_DATA);
		pRenderer->BindConstantBuffer_Internal(ShaderType::COMPUTE_SHADER, { m_pConstantBuffer }, (uint)ConstantBufferIndex::RESERVED_CAMERA_DATA);
	}
}

void Camera::Update()
{
	if (m_DirtyFlag)
	{
		UpdateActiveCameraBuffer();
		m_DirtyFlag = false;
	}
}

void Camera::Render()
{
	// TODO: render post processing effects of current camera
}

void Camera::SetCenterPosition(const Point2f& position)
{
	m_CenterPosition = position;
	m_DirtyFlag = true;
}

void Camera::SetCenterPosition(float x, float y)
{
	m_CenterPosition.x = x;
	m_CenterPosition.y = y;
	m_DirtyFlag = true;
}

void Camera::SetScale(float scale)
{
	m_Scale = std::clamp(scale, m_MinScale, m_MaxScale);
	m_DirtyFlag = true;
}

Rect4f Camera::GetWorldRectangleInView() const
{
	const float scaledWidth = m_Size.x * m_Scale;
	const float scaledHeight = m_Size.y * m_Scale;

	return Rect4f((m_CenterPosition.x) - (scaledWidth / 2), (m_CenterPosition.y) - (scaledHeight / 2), scaledWidth, scaledHeight);
}

void Camera::UpdateActiveCameraBuffer()
{
	CameraBufferElement buffer;
	buffer.width = m_Size.x;
	buffer.height = m_Size.y;
	buffer.scale = m_Scale;
	buffer.positionX = m_CenterPosition.x - ((m_Size.x * m_Scale) / 2);
	buffer.positionY = m_CenterPosition.y - ((m_Size.y * m_Scale) / 2);

	m_pConstantBuffer->UpdateData(&buffer);
}