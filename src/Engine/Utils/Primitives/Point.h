// Folder: Utils/Primitives

#pragma once

template<typename T>
struct Point2
{
	constexpr Point2() = default;
	constexpr Point2(T x, T y)
		: x(x)
		, y(y)
	{}

	constexpr bool operator==(const Point2<T>& rhs) const
	{
		const T epsilon = (T)DBL_EPSILON;
		return std::abs(x - rhs.x) <= epsilon &&
			std::abs(y - rhs.y) <= epsilon;
	}

	constexpr bool operator!=(const Point2<T>& rhs) const
	{
		return !(*this == rhs);
	}

	constexpr Point2<T> operator+(const Point2<T>& rhs) const
	{
		return Point2<T>(x + rhs.x, y + rhs.y);
	}

	constexpr Point2<T>& operator+=(const Point2<T>& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	constexpr Point2<T> operator-(const Point2<T>& rhs) const
	{
		return Point2<T>(x - rhs.x, y - rhs.y);
	}

	constexpr Point2<T>& operator-=(const Point2<T>& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	constexpr Point2<T> operator*(const Point2<T>& rhs) const
	{
		return Point2<T>(x * rhs.x, y * rhs.y);
	}

	constexpr Point2<T>& operator*=(const Point2<T>& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	constexpr Point2<T> operator/(const Point2<T>& rhs) const
	{
		return Point2<T>(x / rhs.x, y / rhs.y);
	}

	constexpr Point2<T>& operator/=(const Point2<T>& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}

	constexpr Point2<T> operator-() const
	{
		return Point2<T>(-x, -y);
	}

	template<typename U>
	constexpr operator Point2<U>() const
	{
		return Point2<U>((U)x, (U)y);
	}

	struct Hasher
	{
		size_t operator()(const Point2<T>& point) const
		{
			size_t hash = 0;
			Utils::HashCombine(hash, point.x);
			Utils::HashCombine(hash, point.y);

			return hash;
		}
	};

	T x;
	T y;
};

using Point2f = Point2<float>;
using Point2i = Point2<int>;