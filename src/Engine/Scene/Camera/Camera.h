// Folder: Scene/Camera

#pragma once

class Buffer;

class Camera
{
	friend class CameraManager;
	friend class DebugCamera;

public:
	Camera();
	Camera(const Point2f& centerPosition, const Point2f& size, float minScale = 0.1f, float maxScale = 2.f);

	virtual ~Camera() = default;

	virtual void Update();
	virtual void Render();

	void SetCenterPosition(const Point2f& position);
	void SetCenterPosition(float x, float y);
	void SetScale(float scale);

	const Point2f& GetCenterPosition() const { return m_CenterPosition; }
	Rect4f GetWorldRectangleInView() const;

	float GetScale() const { return m_Scale; }
	bool IsDirty() const { return m_DirtyFlag; }

	virtual bool IsDebugCamera() const { return false; }

protected:
	void UpdateActiveCameraBuffer();

protected:
	static Buffer* m_pConstantBuffer;

	bool m_DirtyFlag;
	Point2f m_CenterPosition;
	Point2f m_Size;
	float m_MinScale;
	float m_MaxScale;
	float m_Scale;
};