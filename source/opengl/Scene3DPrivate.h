#ifndef Scene3DPrivate_h_INCLUDED
#define Scene3DPrivate_h_INCLUDED

/******************************************************************
Done by Francois Braud, with help of
http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=06
******************************************************************/

#include <cmath>

#define GL_GLEXT_PROTOTYPES
#include <cstdlib>
#ifdef _MSC_VER
	#include <Windows.h>
	#include <WinGDI.h>

	#include <GL/glew.h>
#endif
#include <GL/glut.h>
#ifdef _MSC_VER
	#include <GL/glext.h>
#endif

//---------------------------------------------------------------------

#include "../../include/CoreUtils.h"
#include "../../include/BasicMath.h"
#include "../../include/Camera.h"
#include "../../include/BoundingBoxes.h"
#include "../../include/Utils.h"
#include "../../include/MathUtils.h"
#include "../../include/Obj3D.h"
#include "../../include/Engine.h"
#include "../../include/FileUtil.h"

#include "Shader.h"

class Scene3DPrivate
{
public:
	Scene3DPrivate();

	bool areShadersSupported() const;

	void setRenderOppositeFace(bool yesNo);
	void adjustFog(float distanceToCameraInWorldCoord, float distanceToCameraInCameraCoord);
	bool isFogEnabled() const;
	void setFogDisable();
	void drawCube5FacesWithIntPos(unsigned int textureSidesId, unsigned int textureTopId, unsigned int textureBottomId, const BoundingBoxes::AdvancedBox& box, int depth, bool calculateNormals);
	void drawCylinder(unsigned int textureId, float width, float height, int subDiv1, int subDiv2);
	void updateCameraSyncronisedWith2D(const Int2& virtualWindowSize, Camera& camera);
	void configureLight(int idLight, bool enable, const Float3& pos, float parIntensity);
	void drawFaceWithIntPos(unsigned int textureId, const BoundingBoxes::AdvancedBox& box, bool mirrorX, bool mirrorY);
	void drawCylinderWithIntPos(unsigned int textureId, const Int2& position, float rotationAngle, float width, float height, int subDiv1, int subDiv2);
	void updateLight();
	void set3DMode(Camera& camera);
	void set3DModeLookAtMode(const Float3& parCenter, float distance, Camera& camera);
	void draw3DSegment(const Float3& parSegmentPt1, const Float3& parSegmentPt2);
	void setWireframeMode(bool trueForYes);
	void startDrawingTriangles(const int textureId);
	void stopDrawingTriangles();
	void drawTriangles(const std::vector<Float3>& parPositionPoints, const int textureId, float scaleFactor);
	void drawTriangles_requireStartDrawing(const std::vector<Float3>& parPositionPoints, float scaleFactor);
	bool drawCubeFace(const Float3& parSize, unsigned int textureId, const Float3& parTextureRepeat, int faceToDraw, const Float3& cubePosition, bool doOptimizations, float horizontalFOV, const Camera& camera, float fogMaxDistance);
	void drawCube(const Float3& parSize, const unsigned int parTextureId[6], const Float3& parTextureRepeat, bool reverseToSkyBox);
	void getVerticesAndTexCoordCube(Float3* vertices, Float2* texCoord, const Float3& parTexCoordBegin, const Float3& parTexCoordEnd, const Float3& parTexCoordBeginOtherSide, const Float3& parTexCoordEndOtherSide, const Float3& size, bool randomTexCoord); // must be size 24
	void drawCube2(const Float3& parSize, const unsigned int parTextureId[6], const Float3& parTextureRepeat, bool reverseToSkyBox, bool alphaTestEffect);
	void drawPlane(const Float2& parSize, unsigned int parTextureId, const Float2& parTextureRepeat, const Float3& parAngle, bool reverse);
	void drawQuad(const Float2& parSize, const Float3 pts[4], unsigned int parTextureId, const Float2& parTextureRepeat);
	void pushPosition();
	void popPosition();
	Float3 getPosition() const;
	void rotatePosition(float angleX, float angleY, float angleZ);
	void scalePosition(float x, float y, float z);
	void setPosition(const Float3& parNewPosition);
	void movePosition(const Float3& parMovement);
	unsigned int createAndBeginDrawList();
	void endDrawList();
	void callDrawList(unsigned int dl);
	void removeDrawList(unsigned int dl);
	void startRenderToTexture(float aspectRatio, bool lightingEnable, int parSizeRenderToTexture, Camera& camera);
	void stopRenderToTexture(unsigned int textureId);
	Int2 get2DPosFrom3D(const Float3& pos, const Camera& camera) const;
	Float3 get3DPosFrom2D(const Int2& pos) const;
	bool isLightingEnabled();
	void setPositionAndScaleAndAngle(const Float3& pos, const Float3& scale_, const Float3& angle); //angle in degree
	int getNbTrianglesDrawn() const {return nbTrianglesDrawn;}
	void setMaxVisibleDistance(float parMaxVisibleDistance) {maxVisibleDistance = parMaxVisibleDistance;}
	float getMaxVisibleDistance() {return maxVisibleDistance;}

	const float* getCameraInfos() const { return eyeMatrix; }

	~Scene3DPrivate();
private:
	
	void* quadric;
	float angleDebugFPSVision;
	Int2 previousMousePosition;

	float eyeMatrix[16];

	Float4 m_lightPosition[MAX_LIGHTS];
	bool m_lightIsEnable[MAX_LIGHTS];
	float m_lightIntensity[MAX_LIGHTS];

	float maxVisibleDistance;
	mutable int nbTrianglesDrawn;
};

#endif //Scene3DPrivate_h_INCLUDED
