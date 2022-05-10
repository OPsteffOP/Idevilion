// Folder: Scene

#include "EnginePCH.h"
#include "PrefabFactory.h"

#include "GameObject.h"
#include "RenderComponent.h"
#include "PixelBuffer.h"

GameObject* PrefabFactory::CreateTextureRenderObject(const std::string& texturePath, const Rect4f& destRect)
{
	GameObject* pGameObject = new GameObject();
	pGameObject->SetPosition(Point2f(destRect.x, destRect.y));

	RenderComponent* pRenderComponent = new RenderComponent(texturePath, destRect.width, destRect.height);
	pGameObject->AddComponent(pRenderComponent);

	return pGameObject;
}

GameObject* PrefabFactory::CreateTextureRenderObject(const std::string& texturePath, const Rect4f& destRect, const Rect4f& srcRect)
{
	GameObject* pGameObject = new GameObject();
	pGameObject->SetPosition(Point2f(destRect.x, destRect.y));

	RenderComponent* pRenderComponent = new RenderComponent(texturePath, destRect.width, destRect.height, srcRect);
	pGameObject->AddComponent(pRenderComponent);

	return pGameObject;
}

GameObject* PrefabFactory::CreateTextureRenderObject(PixelBuffer* pPixelBuffer, const Rect4f& destRect, const Rect4f& srcRect)
{
	GameObject* pGameObject = new GameObject();
	pGameObject->SetPosition(Point2f(destRect.x, destRect.y));

	RenderComponent* pRenderComponent = new RenderComponent(pPixelBuffer, destRect.width, destRect.height, srcRect);
	pGameObject->AddComponent(pRenderComponent);

	return pGameObject;
}