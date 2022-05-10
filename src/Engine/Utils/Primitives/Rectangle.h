// Folder: Utils/Primitives

#pragma once

template<typename T>
struct Rect4
{
	constexpr Rect4() = default;
	constexpr Rect4(T x, T y, T width, T height)
		: x(x)
		, y(y)
		, width(width)
		, height(height)
	{}

	bool IsOverlapping(const Rect4<T>& rectangle) const
	{
		return !(x > rectangle.x + rectangle.width ||
			rectangle.x > x + width ||
			y > rectangle.y + rectangle.height ||
			rectangle.y > y + height);
	}

	bool IsInside(const Point2<T>& point) const
	{
		return point.x >= x &&
			point.x < x + width &&
			point.y >= y &&
			point.y < y + height;
	}

	T x;
	T y;
	T width;
	T height;
};

using Rect4f = Rect4<float>;
using Rect4i = Rect4<int>;