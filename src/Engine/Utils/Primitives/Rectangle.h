// Folder: Utils/Primitives

#pragma once

struct Rect4f
{
	Rect4f() = default;
	Rect4f(float x, float y, float width, float height);

	bool IsOverlapping(const Rect4f& rectangle) const;
	bool IsInside(const Point2f& point) const;

	float x;
	float y;
	float width;
	float height;
};