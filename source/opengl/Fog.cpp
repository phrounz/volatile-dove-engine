
#include "GLExtension.h"

#include "Fog.h"

/*#ifdef USES_JS_EMSCRIPTEN
	#define glFogCoordfEXT glFogCoordf
#else*/

#define GL_FOG_COORDINATE_SOURCE_EXT	0x8450
#define GL_FOG_COORDINATE_EXT		0x8451

//typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);
#ifndef USES_JS_EMSCRIPTEN
PFNGLFOGCOORDFEXTPROC glFogCoordfEXT = NULL;
#endif
//#endif

//---------------------------------------------------------------------
//---------------------------------------------------------------------

bool Fog::m_useVolumetricFog = false;

//---------------------------------------------------------------------

void Fog::init()
{
	if (m_useVolumetricFog)
	{
#ifndef USES_JS_EMSCRIPTEN
		if (GLExtension::isExtensionSupported("GL_EXT_fog_coord"))
		{	
			glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC) wglGetProcAddress("glFogCoordfEXT");
		}
		else
		{
			Utils::print("WARNING: extension GL_EXT_fog_coord missing\n");
			Assert(false);
		}
#endif
		glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);//GL_FRAGMENT_DEPTH_EXT);//

		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_EXP);
	}
	else
	{
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_LINEAR);
	}
	
	setFogColor(CoreUtils::colorWhite);
	setRange(0.f, 1.f);
	//glHint(GL_FOG_HINT, GL_DONT_CARE);
	glHint(GL_FOG_HINT, GL_NICEST);
	//glFogi(GL_FOG_COORD_SRC, GL_FOG_COORD);
	
	/*fogMaxDistance = parFogMaxDistance;
	glEnable(GL_FOG);
	
	glFogf(GL_FOG_START, 3.f*fogMaxDistance/4.f);
	glFogf(GL_FOG_END, fogMaxDistance);

	if (exponential)
	{
		glFogf(GL_FOG_DENSITY, 0.001f);
		glFogi (GL_FOG_MODE, GL_EXP2);
	}
	else
	{
		glFogf(GL_FOG_DENSITY, 0.35f);
		glFogi(GL_FOG_MODE, GL_LINEAR);
	}
	
	//glFogf(GL_FOG_COORD_SRC, GL_FOG_COORD);do not work
	float col[4];
	col[0] = parColor.r() / 256.f;
	col[1] = parColor.g() / 256.f;
	col[2] = parColor.b() / 256.f;
	col[3] = parColor.a() / 256.f;
	glFogfv(GL_FOG_COLOR, col);*/
}

//---------------------------------------------------------------------

void Fog::setMinDistance(float parMin)
{
	if (!m_useVolumetricFog) glFogf(GL_FOG_START, parMin);
}

//---------------------------------------------------------------------

void Fog::setMaxDistance(float parMax)
{
	if (!m_useVolumetricFog) glFogf(GL_FOG_END, parMax);
}

//---------------------------------------------------------------------

void Fog::setPos()
{
	glFogCoordfEXT(1.f);
}

//---------------------------------------------------------------------

float Fog::getMinDistance()
{
	float dist;
	glGetFloatv(GL_FOG_START, &dist);
	return dist;
}

//---------------------------------------------------------------------

float Fog::getMaxDistance()
{
	float dist;
	glGetFloatv(GL_FOG_END, &dist);
	return dist;
}

//---------------------------------------------------------------------

void Fog::setFogColor(const Color& parColor)
{
	float col[4];
	col[0] = parColor.r() / 256.f;
	col[1] = parColor.g() / 256.f;
	col[2] = parColor.b() / 256.f;
	col[3] = parColor.a() / 256.f;
	glFogfv(GL_FOG_COLOR, col);
}

//---------------------------------------------------------------------

void Fog::setFogEnable(bool yesNo)
{
	if (yesNo) glEnable(GL_FOG);
	else glDisable(GL_FOG);
}

//---------------------------------------------------------------------

float* Fog::getVolumetricFogCoordinates(int vertexCount, const Float3* vertices)
{
	Assert(m_useVolumetricFog);
	float* fogCoord = new float[ vertexCount ];
	for (int i = 0; i < vertexCount ; ++i)
	{
		float distToOrigin = sqrt(vertices[i].x() * vertices[i].y() * vertices[i].z());
		fogCoord[i] = distToOrigin/300.f;//1000Utils::random(1.f);
	}
	return fogCoord;
}

//---------------------------------------------------------------------
