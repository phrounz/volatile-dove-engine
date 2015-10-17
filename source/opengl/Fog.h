#ifndef Fog_h_INCLUDED
#define Fog_h_INCLUDED

#include "../../include/CoreUtils.h"

typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);

//#ifdef WIN32
//extern PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;
//#endif

//---------------------------------------------------------------------

class Fog
{
public:
	static void init();
	static void setRange(float parMin, float parMax)
	{
		setMinDistance(parMin);
		setMaxDistance(parMax);
	}
	static void setMinDistance(float parMin);
	static void setMaxDistance(float parMax);
	static float getMinDistance();
	static float getMaxDistance();
	static void setFogColor(const Color& parColor);
	static void setFogEnable(bool yesNo);
	static void setPos();

	static bool isVolumetricFog() {return m_useVolumetricFog;}

	static float* getVolumetricFogCoordinates(int vertexCount, const Float3* vertices);
private:
	static bool m_useVolumetricFog;
};

//---------------------------------------------------------------------

#endif //Fog_h_INCLUDED
