
/******************************************************************
Done by Francois Braud excepte si mention contraire
******************************************************************/

#include <limits>
#include <cmath>

#include "../include/MathUtils.h"

#include "../include/Utils.h"

#define M_PI 3.14159265358979323846

namespace MathUtils
{
	//const float PI = (float)M_PI;
	const float PI_OVER_180 = (float)M_PI / 180.f;
	const float ANGLE_180_OVER_PI = (float)180.f / (float)M_PI;
	const float PI_X2 = (float)M_PI * 2.f;

	//---------------------------------------------------------------------
	/**
		Calcule l'inverse d'une racine carree avec une precision de 10E-3
		(jusqu'a 4x plus rapide que sqrt)
		Fonction ecrite par John Carmack pour Quake 3 Arena
		*/

	float fastInverseSqrt(float number)
	{
#ifndef WIN32
		Assert(false);//marche pas sous linux
#endif
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long *)&y;  // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1); // what the fuck?
		y = *(float *)&i;
		y = y * (threehalfs - (x2 * y * y)); // 1st iteration
		// y  = y * ( threehalfs - ( x2 * y * y ) ); // 2nd iteration, this can be removed

		/*
			#ifndef Q3_VM
			#ifdef __linux__
			assert( !isnan(y) ); // bk010122 - FPE?
			#endif
			#endif
			*/
		return y;
	}

	//---------------------------------------------------------------------
	/**
		calcule la racine carree
		*/

	float accurateSqrt(float number)
	{
		return sqrt(number);
	}

	//---------------------------------------------------------------------
	/**
		calcule le sinus avec une certaine imprecision
		http://www.devmaster.net/forums/showthread.php?t=5784
		*/
	const float PI = 3.14159265358979323846264338327950288f;
	const float PI2 = 6.28318530717958647692528676655900577f;
	const float PID2 = 1.57079632679489661923132169163975144f;
	const float PI_SQR = 9.86960440108935861883449099987615114f;

	float fastSin(float x)
	{
		const float B = 4.f / PI;
		const float C = -4.f / PI_SQR;

		return B * x + C * x * fabs(x);
	}

	float fastSinMorePrecise(float x)
	{
		//AssertMessage(false, "untested");
		const float B = 4.f / PI;
		const float C = -4.f / PI_SQR;
		const float P = 0.225f;
		//  const float Q = 0.775;

		float y = B * x + C * x * fabs(x);
		return P * (y * fabs(y) - y) + y;   // Q * y + P * y * abs(y)
	}

	float fastCos(float x)
	{
		return fastSin(PI / 2.f + x);
	}

	//---------------------------------------------------------------------

	float getAngleInPiRange(float angleInRadians)
	{
		Assert(angleInRadians != std::numeric_limits< float >::infinity() && angleInRadians != -std::numeric_limits< float >::infinity());
		//std::cout << angleInRadians*ANGLE_180_OVER_PI << std::endl;
		if (angleInRadians >= PI) angleInRadians -= PI_X2 * ((float)(int)((angleInRadians + PI) / PI_X2));
		else if (angleInRadians < -PI) angleInRadians += PI_X2 * ((float)(int)(-(angleInRadians - PI) / PI_X2));
		//std::cout << angleInRadians*ANGLE_180_OVER_PI << std::endl;
		if (!(angleInRadians < PI && angleInRadians >= -PI))
		{
			//std::cout << angleInRadians << std::endl;
			return 0;//Assert(false);
		}
		return angleInRadians;
	}

	//---------------------------------------------------------------------

	float getAngleIn180Range(float angleInDegree)
	{
		Assert(angleInDegree != std::numeric_limits< float >::infinity() && angleInDegree != -std::numeric_limits< float >::infinity());
		if (angleInDegree >= 180.f) angleInDegree -= 360.f * ((float)(int)((angleInDegree + 180.f) / 360.f));
		else if (angleInDegree < -180.f) angleInDegree += 360.f * ((float)(int)(-(angleInDegree - 180.f) / 360.f));
		if (!(angleInDegree < 180.f && angleInDegree >= -180.f))
			return 0;//Assert(false);
		return angleInDegree;
	}

	//---------------------------------------------------------------------

}