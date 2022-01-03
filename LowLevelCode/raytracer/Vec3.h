#pragma once

#include <fstream>
#include <algorithm>

template<typename T>
class Vec3
{
public:
	Vec3() :
		x(T(0.0f)),
		y(T(0.0f)),
		z(T(0.0f))
	{
	}

	Vec3(T xx) :
		x(xx),
		y(xx),
		z(xx)
	{
	}

	Vec3(T xx, T yy, T zz) :
		x(xx),
		y(yy),
		z(zz)
	{
	}

	void normalize()
	{
		T lengthSquared = length2();
		if (lengthSquared > 0.0f)
		{
			T invNor = 1.0f / sqrt(lengthSquared);

			x *= invNor;
			y *= invNor;
			z *= invNor;
		}
	}

	T dot(const Vec3<T>& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	T length2() const
	{
		return x * x + y * y + z * z;
	}

	T length() const
	{
		return sqrt(length2());
	}

	Vec3<T> operator* (const T& scalar) const
	{
		return Vec3<T>(x * scalar, y * scalar, z * scalar);
	}

	Vec3<T> operator* (const Vec3<T>& vec) const
	{
		return Vec3<T>(x * vec.x, y * vec.y, z * vec.z);
	}

	Vec3<T> operator- (const Vec3<T>& vec) const
	{
		return Vec3<T>(x - vec.x, y - vec.y, z - vec.z);
	}

	Vec3<T> operator+ (const Vec3<T>& v) const
	{
		return Vec3<T>(x + v.x, y + v.y, z + v.z);
	}

	Vec3<T>& operator += (const Vec3<T>& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	Vec3<T>& operator *= (const Vec3<T>& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		
		return *this;
	}

	Vec3<bool> operator < (const Vec3<T>& v) const
	{
		Vec3<bool> result;

		result.x = x < v.x;
		result.y = y < v.y;
		result.z = z < v.z;

		return result;
	}

	Vec3<bool> operator > (const Vec3<T>& v) const
	{
		Vec3<bool> result;

		result.x = x > v.x;
		result.y = y > v.y;
		result.z = z > v.z;

		return result;
	}

	Vec3<bool> operator Vec3<bool>::|| (const Vec3<bool>& v) const
	{
		Vec3<bool> result;

		result.x = x || v.x;
		result.y = y || v.y;
		result.z = z || v.z;

		return result;
	}

	Vec3<T> operator - () const
	{
		return Vec3<T>(-x, -y, -z);
	}
	
	friend std::ostream& operator << (std::ostream& os, const Vec3<T>& v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}

	static bool isAnyTrue(const Vec3<bool>& vec)
	{
		return vec.x || vec.y || vec.z;
	}

	static Vec3<T> min(const Vec3<T>& vec1, const Vec3<T> vec2)
	{
		Vec3<T> minVec = vec1;

		minVec.x = std::min(vec1.x, vec2.x);
		minVec.y = std::min(vec1.y, vec2.y);
		minVec.z = std::min(vec1.z, vec2.z);

		return minVec;
	}

	static Vec3<T> max(const Vec3<T>& vec1, const Vec3<T> vec2)
	{
		Vec3<T> maxVec = vec1;

		maxVec.x = std::max(vec1.x, vec2.x);
		maxVec.y = std::max(vec1.y, vec2.y);
		maxVec.z = std::max(vec1.z, vec2.z);

		return maxVec;
	}

	T x, y, z;
};

typedef Vec3<float> Vec3f;
