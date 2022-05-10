// Folder: Scene

#include "EnginePCH.h"
#include "GameObject.h"

#include "Component.h"
#include "Scene.h"

GameObject::~GameObject()
{
	for (Component* pComponent : m_pComponents)
	{
		SAFE_DELETE(pComponent);
	}

	for (std::pair<Component*, ActionType>& pair : m_Actions)
	{
		if (pair.second == ActionType::ADD || pair.second == ActionType::DELETE)
			SAFE_DELETE(pair.first);
	}
}

void GameObject::Update()
{
	for (Component* pComponent : m_pComponents)
	{
		pComponent->Update();
	}

	ApplyComponentActions();
	m_IsPositionDirty = false;
}

void GameObject::Render()
{
	for (Component* pComponent : m_pComponents)
	{
		pComponent->Render();
	}

	ApplyComponentActions();
}

void GameObject::AddComponent(Component* pComponent)
{
	m_Actions.emplace_back(pComponent, ActionType::ADD);
}

void GameObject::RemoveComponent(Component* pComponent, bool shouldDelete)
{
	m_Actions.emplace_back(pComponent, shouldDelete ? ActionType::DELETE : ActionType::REMOVE);
}

void GameObject::Enable()
{
	if (m_IsEnabled)
		return;

	for (Component* pComponent : m_pComponents)
	{
		pComponent->Activated();
	}

	m_IsEnabled = true;
}

void GameObject::Disable()
{
	if (!m_IsEnabled)
		return;

	for (Component* pComponent : m_pComponents)
	{
		pComponent->Deactivated();
	}

	m_IsEnabled = false;
}

void GameObject::SetPosition(const Point2f& position)
{
	m_Position = position;
	m_IsPositionDirty = true;
}

void GameObject::Move(const Point2f& movement)
{
	m_Position += movement;
	m_IsPositionDirty = true;
}
void GameObject::Move(float movementX, float movementY)
{
	m_Position.x += movementX;
	m_Position.y += movementY;
	m_IsPositionDirty = true;
}

void GameObject::ApplyComponentActions()
{
	std::vector<Component*> pToDeleteComponents;

	for (const std::pair<Component*, ActionType>& pair : m_Actions)
	{
		switch (pair.second)
		{
		case ActionType::ADD:
			m_pComponents.push_back(pair.first);
			pair.first->m_pGameObject = this;
			pair.first->Initialize();
			ActivateComponent(pair.first);
			break;
		case ActionType::DELETE:
			pToDeleteComponents.push_back(pair.first);
			[[fallthrough]]; // Delete action should also erase the component from the gameobject
		case ActionType::REMOVE:
			m_pComponents.erase(std::remove(m_pComponents.begin(), m_pComponents.end(), pair.first));
			pair.first->m_pGameObject = nullptr;
			DeactivateComponent(pair.first);
			break;
		}
	}
	m_Actions.clear();

	for (Component* pComponent : pToDeleteComponents)
	{
		SAFE_DELETE(pComponent);
	}
}

void GameObject::ActivateComponent(Component* pComponent)
{
	if (m_IsEnabled && pComponent->m_pGameObject != nullptr && pComponent->m_pGameObject->m_pParentScene != nullptr)
		pComponent->Activated();
}

void GameObject::DeactivateComponent(Component* pComponent)
{
	// TODO: this has the risk of calling Deactive when the component was already deactivated
	if (!m_IsEnabled || pComponent->m_pGameObject == nullptr || pComponent->m_pGameObject->m_pParentScene == nullptr)
		pComponent->Deactivated();
}