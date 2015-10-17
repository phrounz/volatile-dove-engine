#ifndef Scene3D_h_INCLUDED
#define Scene3D_h_INCLUDED

#include <map>
#include <utility>
#include <vector>

#include "CoreUtils.h"
#include "BasicMath.h"
#include "Camera.h"
#include "BoundingBoxes.h"

class Scene3DPrivate;
class Shader;

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	class AdvancedBox;
	#define MAX_LIGHTS 8
#endif

//---------------------------------------------------------------------
/**
* \brief The manager of the 3D scene (singleton).
*/

class Scene3D
{
	friend class Engine;

public:

	//! Set 3D mode and update camera, using current camera position and angle.
	//! You must update camera before drawing something 3D
	void set3DMode(bool enableLighting);

	void enableLighting(bool yesNo);

	void setWireframeMode(bool trueForYes);

	//--------------
	// fog

	void setFogMinDistance(float newDist);
	void setFogMaxDistance(float newDist);
	void setFogColor(const Color& parColor);
	float getFogMinDistance() const;
	float getFogMaxDistance() const;
	const Color& getFogColor() const;

	//--------------
	// camera

	const Camera& getCamera() const { return m_camera; }
	Camera& getCamera() { return m_camera; }

	//--------------
	// position and angle

	void pushPosition();
	void popPosition();

	Float3 getPosition() const;
	void setPositionAndScaleAndAngle(const Float3& pos, const Float3& scale_, const Float3& angle); //angle in degree
	void setPosition(const Float3& pos);

	//--------------
	// misc

	Int2 get2DPosFrom3D(const Float3& pos) const;
	void enableGet3DPosFrom2D(bool yesNo);
	Float3 get3DPosFrom2D(const Int2& pos, const void* givenIndicesAndVerticesOrNull);
	std::vector<Float3> getAll3DPosFrom2D(const Int2& pos);
	void setRenderOppositeFace(bool yesNo);

	//--------------
	// user-defined texture id

	void setUserDefinedTextureIdFromTextureFile(const std::string& textureFilename, int userDefinedTextureId)
	{
		m_userDefinedTextureIdFromTextureFile[textureFilename] = userDefinedTextureId;
	}
	void removeUserDefinedTextureIdFromTextureFile(const std::string& textureFilename)
	{
		m_userDefinedTextureIdFromTextureFile.erase(textureFilename);
	}
	void useUserDefinedTextureIdIfAvailable(const std::string& textureFilename);

	void updateMiscShaderData(float value0, float value1, float value2, bool updateValue0, bool updateValue1, bool updateValue2);

private:
	Scene3D();
	~Scene3D();

	Camera m_camera;
	Float3 m_pos;

	Scene3DPrivate* m_private;

	std::map<std::string, int> m_userDefinedTextureIdFromTextureFile;

	bool m_enablePicking;
	Shader* m_shader;

//---------------------------------------------------------------------

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
public:

	void setPositionAndScale(const Float3& pos, const Float3& scale);
	void clearScene(const Color& backgroundColor);
	//void updateCameraLowLevel();
	//void setRenderBlurLevel(int blurLevel);

private:
	
	void createWindowSizeDependentResources();

#endif
	
//---------------------------------------------------------------------

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
public:
	void setMaxVisibleDistance(float parMaxVisibleDistance);
	float getMaxVisibleDistance();

	//! Get key events for moving camera with u,h,j,k,o,l,z
	//void injectKeyForDebug(unsigned char key);
	//void injectKeyForDebugCollision(unsigned char key, Object3D* model);
	//void injectKeyForDebugFPSCollision(unsigned char key, Object3D* model);

	//! Get Field of view
	float getHorizontalFOV() const;

	//void setRenderOppositeFace(bool yesNo);

	
	void adjustFog(float distanceToCameraInWorldCoord, float distanceToCameraInCameraCoord);
	bool isFogEnabled() const;
	void setFogDisable();

	//! Camera
	//void setSlowlyCameraAngle(Float3 parAngleCamera);

	
	void set3DModeLookAtMode(bool enableLighting, const Float3& parCenter, float distance);
	//float updateCameraDebugFPSVisionAndManageFall(Object3D* model);
	void updateCameraSyncronisedWith2D(const Int2& virtualWindowSize);
	
	//void injectMouseMotionForCameraAngle(const Int2& parMousePosition, bool parIsClicking);
	
	//! Drawing primitives using an advanced box or integer positions. Useful for set3DModeSyncronisedWith2D mode.
	void drawCube5FacesWithIntPos(unsigned int textureSidesId, unsigned int textureTopId, unsigned int textureBottomId, const BoundingBoxes::AdvancedBox& box, int depth, bool calculateNormals);
    void drawFaceWithIntPos(unsigned int textureId, const BoundingBoxes::AdvancedBox& box, bool mirrorX, bool mirrorY);
	void drawCylinderWithIntPos(unsigned int textureId, const Int2& position, float rotationAngle, float width, float height, int subDiv1, int subDiv2);
	
	//! Drawing primitives
	void drawCylinder(unsigned int textureId, float width, float height, int subDiv1, int subDiv2);
	void draw3DSegment(const Float3& parSegmentPt1, const Float3& parSegmentPt2);
	void drawTriangles(const std::vector<Float3>& parPositionPoints, const int textureId, float scaleFactor = 1.f);
	void drawTriangles_requireStartDrawing(const std::vector<Float3>& parPositionPoints, float scaleFactor);
	void startDrawingTriangles(const int textureId);
	void stopDrawingTriangles();
	//parTextureId: top, bottom, front, back, left, right
	void drawCube(
		const Float3& parSize, 
		const unsigned int parTextureId[6] = NULL, 
		const Float3& parTextureRepeat = Float3(1.f,1.f,1.f), 
		bool reverseToSkyBox = false);
	void drawCube2(
		const Float3& parSize, 
		const unsigned int parTextureId[6] = NULL, 
		const Float3& parTextureRepeat = Float3(1.f,1.f,1.f), 
		bool reverseToSkyBox = false,
		bool alphaTestEffect = false);
	bool drawCubeFace(
		const Float3& parSize, unsigned int textureId, const Float3& parTextureRepeat, 
		int faceToDraw, const Float3& cubePosition, bool doOptimizations);

	void getVerticesAndTexCoordCube(Float3* vertices, Float2* texCoord, const Float3& parTexCoordBegin, const Float3& parTexCoordEnd, const Float3& parTexCoordBeginOtherSide, const Float3& parTexCoordEndOtherSide, const Float3& size, bool randomTexCoord); // vertices must be size 24
	
	void drawPlane(
		const Float2& parSize, 
		unsigned int parTextureId = -1, 
		const Float2& parTextureRepeat = Float2(1.f,1.f), 
		const Float3& parAngle = Float3(0,0,0),
		bool reverse = false);

	void drawQuad(const Float2& parSize, const Float3 pts[4], unsigned int parTextureId, const Float2& parTextureRepeat);
	
	//! Move in 3D world
	
	void movePosition(const Float3& parMovement);
	void rotatePosition(float angleX, float angleY, float angleZ);//in degree
	void scalePosition(float x, float y, float z);
	
	//if parIntensity == 0.f: light has no attenuation
	void configureLight(int idLight, bool enable, const Float3& pos, float parIntensity);

	unsigned int createAndBeginDrawList();
	void endDrawList();
	void callDrawList(unsigned int dl);
	void removeDrawList(unsigned int dl);

	int getNbTrianglesDrawn() const;

	void startRenderToTexture(float aspectRatio, bool lightingEnable, int parSizeRenderToTexture);
	void stopRenderToTexture(unsigned int textureId);

private:
	void updateLight();
#endif

};

#endif //Scene3D_h_INCLUDED
