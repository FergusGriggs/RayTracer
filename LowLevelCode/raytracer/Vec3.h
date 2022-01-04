#pragma once

#include <fstream>
#include <algorithm>

#include "static_functions.h"

template<typename T>
class Vec3
{
public:
	T x, y, z;

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

	Vec3<bool> operator <= (const Vec3<T>& v) const
	{
		Vec3<bool> result;

		result.x = x <= v.x;
		result.y = y <= v.y;
		result.z = z <= v.z;

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

	Vec3<bool> operator >= (const Vec3<T>& v) const
	{
		Vec3<bool> result;

		result.x = x >= v.x;
		result.y = y >= v.y;
		result.z = z >= v.z;

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

	static float dot(const Vec3<float>& vec1, const Vec3<float>& vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}

	static Vec3<T> normalise(const Vec3<float>& vec)
	{
		T lengthSquared = vec.length2();
		if (lengthSquared < 0.00001f)
		{
			return Vec3<T>(0, 1, 0);
		}

		T invNor = 1.0f / sqrt(lengthSquared);

		return Vec3<T>(vec.x * invNor, vec.y * invNor, vec.z * invNor);
	}

	static Vec3<float> reflect(const Vec3<float>& incident, const Vec3<float>& normal)
	{
		return incident - normal * 2.0f * Vec3<float>::dot(normal, incident);
	}

	static Vec3<T> min(const Vec3<T>& vec1, const Vec3<T>& vec2)
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

	static bool isAnyTrue(const Vec3<bool>& vec)
	{
		return vec.x || vec.y || vec.z;
	}

	static bool areAllTrue(const Vec3<bool>& vec)
	{
		return vec.x && vec.y && vec.z;
	}

	static Vec3<float> randomNegativeOneToOne()
	{
		Vec3<float> randomVec;

		randomVec.x = static_functions::randomNegativeOneToOne();
		randomVec.y = static_functions::randomNegativeOneToOne();
		randomVec.z = static_functions::randomNegativeOneToOne();

		return randomVec;
	}

	static Vec3<float> randomInUnitSphere()
	{
		while (true)
		{
			Vec3<float> point = Vec3<float>::randomNegativeOneToOne();
			if (point.length2() >= 1.0f)
			{
				continue;
			}
			return point;
		}
	}

	static Vec3<float> randomDirection()
	{
		Vec3<float> randomInSphere = randomInUnitSphere();
		randomInSphere.normalize();
		return randomInSphere;
	}

	/*Vec3<bool> operator Vec3<bool>:: || (const Vec3<bool>&v) const
	{
		Vec3<bool> result;

		result.x = x || v.x;
		result.y = y || v.y;
		result.z = z || v.z;

		return result;
	}

	Vec3<bool> operator Vec3<bool>:: && (const Vec3<bool>& v) const
	{
		Vec3<bool> result;

		result.x = x && v.x;
		result.y = y && v.y;
		result.z = z && v.z;

		return result;
	}*/

	static Vec3<bool> boolVecOr(const Vec3<bool>&vec1, const Vec3<bool>& vec2)
	{
		Vec3<bool> result;

		result.x = vec1.x || vec2.x;
		result.y = vec1.y || vec2.y;
		result.z = vec1.z || vec2.z;

		return result;
	}

	static Vec3<bool> boolVecAnd(const Vec3<bool>& vec1, const Vec3<bool>& vec2)
	{
		Vec3<bool> result;

		result.x = vec1.x && vec2.x;
		result.y = vec1.y && vec2.y;
		result.z = vec1.z && vec2.z;

		return result;
	}
};

typedef Vec3<float> Vec3f;