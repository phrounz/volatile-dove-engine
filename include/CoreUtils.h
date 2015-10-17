/******************************************************************
Nouveaux types
Done by Francois Braud
******************************************************************/
#ifndef Core_h_INCLUDED
#define Core_h_INCLUDED

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#ifdef USES_WINDOWS_OPENGL
	#include "stdint.h"
#else
	#include <stdint.h>
#endif

#include "Utils.h"
#include "MathUtils.h"

typedef unsigned char u8;

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t i32;
typedef int64_t i64;
typedef int16_t i16;
typedef float f32;
typedef double f64;

#define AssertCore( x )

namespace CoreUtils
{
	//---------------------------------------------------------------------
	/**
	* \brief A template vector used to store and manage pair, triplet, etc of values. 
	* See also the typedef as Int2,Int3,Float2,Float3,Color,etc.
	*/

	template<u32 nbElements, typename TypeOfElement>
	class BasicVector
	{
	public:
		TypeOfElement data[nbElements];

		BasicVector() {}
		BasicVector(float initialValueForEachElement)
		{
			for (u32 i = 0; i < nbElements; ++i)
				data[i] = (TypeOfElement)initialValueForEachElement;
		}
		BasicVector(const BasicVector<nbElements, TypeOfElement>& b)
		{
			memcpy(data, b.data, nbElements * sizeof(TypeOfElement));
		}

		void set(const BasicVector<nbElements, TypeOfElement>& b)
		{
			memcpy(data, b.data, nbElements * sizeof(TypeOfElement));
		}

		BasicVector<nbElements, TypeOfElement>& operator=(const BasicVector<nbElements, TypeOfElement>& b)
		{
			memcpy(data, b.data, nbElements * sizeof(TypeOfElement));
			return *this;
		}

		BasicVector<nbElements, TypeOfElement>& operator+=(const BasicVector<nbElements, TypeOfElement>& b)
		{
			for (u32 i = 0; i < nbElements; ++i)
				data[i] += b.data[i];
			return *this;
		}

		BasicVector<nbElements, TypeOfElement>& operator-=(const BasicVector<nbElements, TypeOfElement>& b)
		{
			for (u32 i = 0; i < nbElements; ++i)
				data[i] -= b.data[i];
			return *this;
		}

		BasicVector<nbElements, TypeOfElement> operator*(float value) const
		{
			BasicVector<nbElements, TypeOfElement> result;
			for (u32 i = 0; i < nbElements; ++i)
				result.data[i] = (TypeOfElement)((float)data[i] * value);
			return result;
		}

		BasicVector<nbElements, TypeOfElement> operator*(const BasicVector<nbElements, TypeOfElement>& b) const
		{
			BasicVector<nbElements, TypeOfElement> result;
			for (u32 i = 0; i < nbElements; ++i)
				result.data[i] = data[i] * b.data[i];
			return result;
		}

		BasicVector<nbElements, TypeOfElement> operator/(float value) const
		{
			BasicVector<nbElements, TypeOfElement> result;
			for (u32 i = 0; i < nbElements; ++i)
				result.data[i] = (TypeOfElement)((float)data[i] / value);
			return result;
		}

		BasicVector<nbElements, TypeOfElement> operator/(const BasicVector<nbElements, TypeOfElement>& b) const
		{
			BasicVector<nbElements, TypeOfElement> result;
			for (u32 i = 0; i < nbElements; ++i)
				result.data[i] = data[i] / b.data[i];
			return result;
		}

		BasicVector<nbElements, TypeOfElement> operator%(int value) const
		{
			BasicVector<nbElements, TypeOfElement> result;
			for (u32 i = 0; i < nbElements; ++i)
				result.data[i] = data[i] % value;
			return result;
		}

		BasicVector(TypeOfElement first, TypeOfElement second)
		{
			AssertCore(nbElements >= 2);
			data[0] = first;
			data[1] = second;
		}
		BasicVector(TypeOfElement first, TypeOfElement second, TypeOfElement third)
		{
			AssertCore(nbElements >= 3);
			data[0] = first;
			data[1] = second;
			data[2] = third;
		}
		BasicVector(TypeOfElement r, TypeOfElement g, TypeOfElement b, TypeOfElement a)
		{
			AssertCore(nbElements >= 4);
			data[0] = r;
			data[1] = g;
			data[2] = b;
			data[3] = a;
		}
		BasicVector(TypeOfElement* table, size_t nbElementsTable)
		{
			AssertCore(nbElementsTable <= nbElements);
			memcpy(data, table, nbElementsTable * sizeof(TypeOfElement));
		}

		TypeOfElement sumOfElements()
		{
			TypeOfElement sum = 0;
			for (u32 i = 0; i < nbElements; ++i)
				sum += data[i];
			return sum;
		}

		float norm() const
		{
			TypeOfElement squareSum = 0;
			for (u32 i = 0; i < nbElements; ++i)
				squareSum += data[i] * data[i];
			return MathUtils::accurateSqrt(static_cast<float>(squareSum));
		}

		void normalize()
		{
			TypeOfElement n = norm();
			for (u32 i = 0; i < nbElements; ++i)
				data[i] /= n;
		}

		inline TypeOfElement left() const { AssertCore(nbElements == 2); return data[0]; }
		inline TypeOfElement right() const { AssertCore(nbElements == 2); return data[1]; }

		inline TypeOfElement width() const { AssertCore(nbElements == 2); return data[0]; }
		inline TypeOfElement height() const { AssertCore(nbElements == 2); return data[1]; }

		inline TypeOfElement x() const { AssertCore(nbElements >= 1); return data[0]; }
		inline TypeOfElement y() const { AssertCore(nbElements >= 2); return data[1]; }
		inline TypeOfElement z() const { AssertCore(nbElements >= 3); return data[2]; }
		inline TypeOfElement w() const { AssertCore(nbElements >= 4); return data[3]; }

		inline void setX(TypeOfElement value) { AssertCore(nbElements >= 1); data[0] = value; }
		inline void setY(TypeOfElement value) { AssertCore(nbElements >= 2); data[1] = value; }
		inline void setZ(TypeOfElement value) { AssertCore(nbElements >= 3); data[2] = value; }

		inline void setWidth(TypeOfElement value) { AssertCore(nbElements == 2); data[0] = value; }
		inline void setHeight(TypeOfElement value) { AssertCore(nbElements == 2); data[1] = value; }

		inline TypeOfElement r() const { AssertCore(nbElements >= 3); return data[0]; }
		inline TypeOfElement g() const { AssertCore(nbElements >= 3); return data[1]; }
		inline TypeOfElement b() const { AssertCore(nbElements >= 3); return data[2]; }
		inline TypeOfElement a() const { AssertCore(nbElements >= 4); return data[3]; }

		inline float distanceTo(const BasicVector<nbElements, TypeOfElement>& b) const
		{
			TypeOfElement result(0);
			for (u32 i = 0; i < nbElements; ++i)
			{
				TypeOfElement tmp = (data[i] - b.data[i]);
				result += tmp * tmp;
			}

			return MathUtils::accurateSqrt((float)result);
		}

		inline TypeOfElement dotProduct(const BasicVector<nbElements, TypeOfElement>& b) const
		{
			TypeOfElement result(0);
			for (u32 i = 0; i < nbElements; ++i)
			{
				result += data[i] * b.data[i];
			}
			return result;
		}

		std::string toStr() const
		{
			std::stringstream sstr;
			sstr << *this;
			return sstr.str();
		}
	};

	template<u32 nbElements2, typename TypeOfElement2>
	BasicVector<nbElements2, TypeOfElement2> operator+(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		BasicVector<nbElements2, TypeOfElement2> result;
		for (u32 i = 0; i < nbElements2; ++i)
			result.data[i] = a.data[i] + b.data[i];
		return result;
	}

	template<u32 nbElements2, typename TypeOfElement2>
	BasicVector<nbElements2, TypeOfElement2> operator-(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		BasicVector<nbElements2, TypeOfElement2> result;
		for (u32 i = 0; i < nbElements2; ++i)
			result.data[i] = a.data[i] - b.data[i];
		return result;
	}

	template<u32 nbElements2, typename TypeOfElement2>
	bool operator>(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		for (u32 i = 0; i < nbElements2; ++i)
			if (a.data[i] <= b.data[i])
				return false;
		return true;
	}

	template<u32 nbElements2, typename TypeOfElement2>
	bool operator<(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		for (u32 i = 0; i < nbElements2; ++i)
			if (a.data[i] >= b.data[i])
				return false;
		return true;
	}

	template<u32 nbElements2, typename TypeOfElement2>
	bool operator<=(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		for (u32 i = 0; i < nbElements2; ++i)
			if (a.data[i] > b.data[i])
				return false;
		return true;
	}

	template<u32 nbElements2, typename TypeOfElement2>
	bool operator>=(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		for (u32 i = 0; i < nbElements2; ++i)
			if (a.data[i] < b.data[i])
				return false;
		return true;
	}

	template<u32 nbElements2, typename TypeOfElement2>
	bool operator==(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		for (u32 i = 0; i < nbElements2; ++i)
			if (a.data[i] != b.data[i])
				return false;
		return true;
	}

	template<u32 nbElements2, typename TypeOfElement2>
	bool operator!=(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		for (u32 i = 0; i < nbElements2; ++i)
			if (a.data[i] != b.data[i])
				return true;
		return false;
	}

	template<u32 nbElements2, typename TypeOfElement2>
	std::ostream& operator << (std::ostream& flux, const BasicVector<nbElements2, TypeOfElement2>& bVect)
	{
		flux << "BasicVector<" << nbElements2 << ",?(sizeof=" << sizeof(TypeOfElement2) << ")> (";
		for (u32 i = 0; i < nbElements2; ++i)
			flux << bVect.data[i] << ";";
		flux << ")";
		return flux;
	}
}

//---------------------------------------------------------------------

typedef CoreUtils::BasicVector<2, f64> Double2;
typedef CoreUtils::BasicVector<2, f32> Float2;
typedef CoreUtils::BasicVector<3, f32> Float3;
typedef CoreUtils::BasicVector<4, f32> Float4;
typedef CoreUtils::BasicVector<2, i32> Int2;
typedef CoreUtils::BasicVector<3, i32> Int3;
typedef CoreUtils::BasicVector<4, i32> Int4;
typedef CoreUtils::BasicVector<4, u8> UnsignedChar4;
typedef UnsignedChar4 Color;

namespace CoreUtils
{
	float distanceBetweenInt2(const Int2& first, const Int2 second);

	float distanceBetween(const Float3& v1, const Float3& v2);

	Float3 middleBetweenTwoPoints(const Float3& pt1, const Float3& pt2);

	Float3 crossProduct(const Float2& v1, const Float2& v2);
	Float3 crossProduct(const Float3& v1, const Float3& v2);

	float dotProduct(const Float3& v1, const Float3& v2);

	template<u32 nbElements, typename TypeOfElement1, typename TypeOfElement2>
	BasicVector<nbElements, TypeOfElement2> basicVectorCast(const BasicVector<nbElements, TypeOfElement1>& parInput)
	{
		BasicVector<nbElements, TypeOfElement2> result;
		for (int i = 0; i < nbElements; ++i)
		{
			result.data[i] = (TypeOfElement2)parInput.data[i];
		}
		return result;
	}

	//input parameters are in radians
	Float3 get3DCoordinatesFromSphericalCoordinates(float radius, float anglePhi, float angleTheta);
	Float3 get3DCoordinatesFromSphericalCoordinatesDegree(float radius, float anglePhi, float angleTheta);

	//output parameters are in radians
	//return Float3(radius,anglePhi,angleTheta)
	Float3 getSphericalCoordinatesFrom3DCoordinates(const Float3& pos);

	// convert Float3 to Int3 (uses floor() or ceil() on each dimension)
	Int3 fromFloat3ToInt3Floored(const Float3& val);
	Int3 fromFloat3ToInt3Ceiled(const Float3& val);
	Int2 fromDouble2ToInt2(const Double2& val);
	Double2 fromInt2ToDouble2(const Int2& val);
	Float2 fromInt2ToFloat2(const Int2& val);

	// convert Int3 to Float3
	Float3 fromInt3ToFloat3(const Int3& val);

	template<u32 nbElements2, typename TypeOfElement2>
	BasicVector<nbElements2, TypeOfElement2> minimum(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		BasicVector<nbElements2, TypeOfElement2> result;
		for (u32 i = 0; i < nbElements2; ++i)
			result.data[i] = a.data[i] < b.data[i] ? a.data[i] : b.data[i];
		return result;
	}

	template<u32 nbElements2, typename TypeOfElement2>
	BasicVector<nbElements2, TypeOfElement2> maximum(const BasicVector<nbElements2, TypeOfElement2>& a, const BasicVector<nbElements2, TypeOfElement2>& b)
	{
		BasicVector<nbElements2, TypeOfElement2> result;
		for (u32 i = 0; i < nbElements2; ++i)
			result.data[i] = a.data[i] > b.data[i] ? a.data[i] : b.data[i];
		return result;
	}

	/**
	* \brief A pair of Int2 representing a 2D rectangle zone.
	*/

	class RectangleInt
	{
	public:
		RectangleInt() {}
		RectangleInt(const Int2& posTopLeft, const Int2& posBottomRight) : topLeft(posTopLeft), bottomRight(posBottomRight) {}
		bool isIn(const Int2& pt) const { return pt >= topLeft && pt < bottomRight; }
		bool isColliding(const RectangleInt& other) const
		{
			return other.topLeft.y() < bottomRight.y() && other.bottomRight.y() > topLeft.y() && other.topLeft.x() < bottomRight.x() && other.bottomRight.x() > topLeft.x();
		}
		Int2 topLeft;
		Int2 bottomRight;
	};

	RectangleInt operator+(const RectangleInt& a, const Int2& b);
	RectangleInt operator-(const RectangleInt& a, const Int2& b);

	extern Color colorBlack;
	extern Color colorGrey;
	extern Color colorWhite;
	extern Color colorSemiTransparent;
	extern Color colorTransparent;
	extern Color colorBlue;
	extern Color colorRed;
	extern Color colorYellow;
	extern Color colorGreySemiTransparent;
}

typedef unsigned int uint;

//---------------------------------------------------------------------

#endif //Core_h_INCLUDED
