// Folder: Scene/Components

#pragma once

class GameObject;

class Component
{
	friend class GameObject;

public:
	virtual ~Component() = default;

	virtual void Initialize() {}

	virtual void Update() {}
	virtual void Render() {}

	virtual void Activated() {}
	virtual void Deactivated() {}

protected:
	GameObject* m_pGameObject;
};