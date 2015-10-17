/******************************************************************
Nouveaux types
Done by Francois Braud
******************************************************************/

#include <cmath>
#include <limits>

#include "../include/CoreUtils.h"

namespace CoreUtils
{
	Color colorBlack(0,0,0, 255);
	Color colorWhite(255,255,255, 255);
	Color colorSemiTransparent(255,255,255, 128);
	Color colorTransparent(255,255,255,0);
	Color colorBlue(0,0,255,255);
	Color colorYellow(255, 255, 0, 255);
	Color colorRed(255,0,0,255);
	Color colorGrey(128,128,128,255);
	Color colorGreySemiTransparent(128, 128, 128, 128);

	float distanceBetweenInt2(const Int2& first, const Int2 second)
	{
		int diffX = first.x() - second.x();
		int diffY = first.y() - second.y();
		return (float)sqrt((float)(diffX * diffX + diffY * diffY));
	}


	Float3 crossProduct(const Float3& v1, const Float3& v2)
	{
		Float3 result;
		result.data[0] = v1.y() * v2.z() - v1.z() * v2.y();
		result.data[1] = v1.z() * v2.x() - v1.x() * v2.z();
		result.data[2] = v1.x() * v2.y() - v1.y() * v2.x();
		return result;
	}


	Float3 crossProduct(const Float2& v1, const Float2& v2)
	{
		Float3 result;
		result.data[0] = 0;
		result.data[1] = 0;
		result.data[2] = v1.x() * v2.y() - v1.y() * v2.x();
		return result;
	}

	float dotProduct(const Float3& v1, const Float3& v2)
	{
		return v1.data[0] * v2.data[0] + v1.data[1] * v2.data[1] + v1.data[2] * v2.data[2];
	}

	Float3 middleBetweenTwoPoints(const Float3& pt1, const Float3& pt2)
	{
		return (pt1 + pt2) / 2.f;
	}

	float distanceBetween(const Float3& v1, const Float3& v2)
	{
		Float3 tmp = v1 - v2;
		return sqrt(tmp.data[0] * tmp.data[0] + tmp.data[1] * tmp.data[1] + tmp.data[2] * tmp.data[2]);
	}

	//---------------------------------------------------------------------

	Float3 get3DCoordinatesFromSphericalCoordinates(float radius, float anglePhi, float angleTheta)
	{
		//float angleGamma = 1.57079632679489661923f-anglePhi;
		return Float3(
			radius * cos(angleTheta) * cos(anglePhi),
			radius * sin(anglePhi),
			radius * sin(angleTheta) * cos(anglePhi));
	}

	//---------------------------------------------------------------------

	Float3 get3DCoordinatesFromSphericalCoordinatesDegree(float radius, float anglePhi, float angleTheta)
	{
		return get3DCoordinatesFromSphericalCoordinates(radius, anglePhi * MathUtils::PI / 180.f, angleTheta * MathUtils::PI / 180.f);
	}

	//---------------------------------------------------------------------

	Float3 getSphericalCoordinatesFrom3DCoordinates(const Float3& pos)
	{
		float radius = sqrt(pos.x()*pos.x() + pos.y()*pos.y() + pos.z()*pos.z());
		float ac =  acos(pos.x()/sqrt(pos.x()*pos.x()+pos.z()*pos.z()));
		if (pos.x() == 0.f || pos.z() == 0.f)
			return Float3(0.f,0.f,0.f);
		return Float3(
			radius,
			acos(pos.y()/radius),
			pos.z() >= 0 ? ac : (2 * MathUtils::PI - ac));
	}

	//---------------------------------------------------------------------
		
	Int3 fromFloat3ToInt3Floored(const Float3& val)
	{
		Int3 res;
		res.data[0] = (int)floor(val.data[0]);
		res.data[1] = (int)floor(val.data[1]);
		res.data[2] = (int)floor(val.data[2]);
		return res;
	}

	//---------------------------------------------------------------------

	Int3 fromFloat3ToInt3Ceiled(const Float3& val)
	{
		Int3 res;
		res.data[0] = (int)ceil(val.data[0]);
		res.data[1] = (int)ceil(val.data[1]);
		res.data[2] = (int)ceil(val.data[2]);
		return res;
	}

	//---------------------------------------------------------------------

	Float3 fromInt3ToFloat3(const Int3& val)
	{
		Float3 res;
		res.data[0] = (float)val.data[0];
		res.data[1] = (float)val.data[1];
		res.data[2] = (float)val.data[2];
		return res;
	}

	//---------------------------------------------------------------------

	Int2 fromDouble2ToInt2(const Double2& val)
	{
		return Int2((int)val.x(), (int)val.y());
	}

	//---------------------------------------------------------------------

	Double2 fromInt2ToDouble2(const Int2& val)
	{
		return Double2((double)val.x(), (double)val.y());
	}

	//---------------------------------------------------------------------
	
	Float2 fromInt2ToFloat2(const Int2& val)
	{
		return Float2((float)val.x(), (float)val.y());
	}

	//---------------------------------------------------------------------

	RectangleInt operator+(const RectangleInt& a, const Int2& b)
	{
		return CoreUtils::RectangleInt(a.topLeft + b, a.bottomRight + b);
	}

	//---------------------------------------------------------------------

	RectangleInt operator-(const RectangleInt& a, const Int2& b)
	{
		return CoreUtils::RectangleInt(a.topLeft - b, a.bottomRight - b);
	}

	//---------------------------------------------------------------------
}
