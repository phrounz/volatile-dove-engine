
/******************************************************************
Done by Francois Braud
******************************************************************/

#ifndef MathUtils_h_INCLUDED
#define MathUtils_h_INCLUDED

#ifdef _MSC_VER
	#define _USE_MATH_DEFINES
#endif
#include <cmath>

//---------------------------------------------------------------------

namespace MathUtils
{
	extern const float PI;//3.14...
	extern const float PI_OVER_180;
	extern const float ANGLE_180_OVER_PI;
	extern const float PI_X2;//pi*2

    //! renvoie la valeur n encadree par deux valeurs
    inline int inRange(int n, int min, int max)
    {
        return n<min?min:(n>max?max:n);
    }

    //! renvoie le minimum de deux valeurs
    template<typename T>
    inline const T& minimum(const T& first, const T& second)
    {
        return first < second ? first : second;
    }

    //! renvoie le maximum de deux valeurs
    template<typename T>
    inline const T& maximum(const T& first, const T& second)
    {
        return first > second ? first : second;
    }

    //! calcule la racine carree
    float accurateSqrt( float number);

    //! calcule l'inverse d'une racine carree
    //! precis a 10E-3 mais rapide
    float fastInverseSqrt( float number );

    //! methodes de calcul rapide de sinus
    float fastSin(float x);
    float fastSinMorePrecise(float x);

    float fastCos(float x);

	inline float roundFloat(float x) {return floor(x + 0.5f);}

	//return same angle between -pi and pi
	float getAngleInPiRange(float angleInRadians);
	float getAngleIn180Range(float angleInDegree);
}

//---------------------------------------------------------------------

#endif //MathUtils_h_INCLUDED
