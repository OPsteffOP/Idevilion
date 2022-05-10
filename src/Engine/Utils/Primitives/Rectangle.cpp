// Folder: Utils/Primitives

#include "EnginePCH.h"
#include "Rectangle.h"

Rect4f::Rect4f(float x, float y, float width, float height)
	: x(x)
	, y(y)
	, width(width)
	, height(height)
{}

bool Rect4f::IsOverlapping(const Rect4f& rectangle) const
{
	return !(x > rectangle.x + rectangle.width ||
		rectangle.x > x + width ||
		y > rectangle.y + rectangle.height ||
		rectangle.y > y + height);
}

bool Rect4f::IsInside(const Point2f& point) const
{
	return point.x >= x &&
		point.x <= x + width &&
		point.y >= y &&
		point.y <= y + height;
}