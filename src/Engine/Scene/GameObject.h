// Folder: Scene

#pragma once

class Component;
class Scene;

class GameObject
{
	friend class Scene;

public:
	~GameObject();

	void Update();
	void Render();

	void AddComponent(Component* pComponent);
	void RemoveComponent(Component* pComponent, bool shouldDelete = true);

	void Enable();
	void Disable();

	template<typename T>
	T* GetComponent() const
	{
		// TODO: optimize

		for (Component* pComponent : m_pComponents)
		{
			T* pCastedComponent = dynamic_cast<T*>(pComponent);
			if (pCastedComponent != nullptr)
				return pCastedComponent;
		}

		return nullptr;
	}

	template<typename T>
	std::vector<T*> GetComponents() const
	{
		// TODO: optimize

		std::vector<T*> pComponents;

		for (Component* pComponent : m_pComponents)
		{
			T* pCastedComponent = dynamic_cast<T*>(pComponent);
			if (pCastedComponent != nullptr)
				pComponents.push_back(pCastedComponent);
		}

		return pComponents;
	}

#ifdef BP_CONFIGURATION_DEBUG
	void SetDebugName(const std::string& name) { m_DebugName = name; }
#endif

	bool IsPositionDirty() const { return m_IsPositionDirty; }
	const Point2f& GetPosition() const { return m_Position; }
	void SetPosition(const Point2f& position);
	
	void Move(const Point2f& movement);
	void Move(float movementX, float movementY);

private:
	void ApplyComponentActions();
	void ActivateComponent(Component* pComponent);
	void DeactivateComponent(Component* pComponent);

private:
#ifdef BP_CONFIGURATION_DEBUG
	std::string m_DebugName;
#endif

	bool m_IsEnabled;

	enum class ActionType
	{
		ADD,
		REMOVE,
		DELETE
	};

	bool m_IsPositionDirty;
	Point2f m_Position;

	std::vector<std::pair<Component*, ActionType>> m_Actions;
	std::vector<Component*> m_pComponents;

	Scene* m_pParentScene;
};