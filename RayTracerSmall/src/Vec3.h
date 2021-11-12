#pragma once

#include <fstream>

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

	Vec3<T> operator - () const
	{
		return Vec3<T>(-x, -y, -z);
	}
	
	friend std::ostream& operator << (std::ostream& os, const Vec3<T>& v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}

	T x, y, z;
};

typedef Vec3<float> Vec3f;
