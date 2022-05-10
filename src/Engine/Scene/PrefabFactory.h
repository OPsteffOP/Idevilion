// Folder: Scene

#pragma once

class GameObject;
class PixelBuffer;

namespace PrefabFactory
{
	GameObject* CreateTextureRenderObject(const std::string& texturePath, const Rect4f& destRect);
	GameObject* CreateTextureRenderObject(const std::string& texturePath, const Rect4f& destRect, const Rect4f& srcRect);
	GameObject* CreateTextureRenderObject(PixelBuffer* pPixelBuffer, const Rect4f& destRect, const Rect4f& srcRect);

	//GameObject* CreateStaticForegroundObject(const std::string& texturePath, float destWidth, float destHeight);
}