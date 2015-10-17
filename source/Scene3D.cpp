

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)

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

//#define GL_FOG_COORDINATE_SOURCE_EXT	0x8450					// Value Taken From GLEXT.H
//#define GL_FOG_COORDINATE_EXT		0x8451					// Value Taken From GLEXT.H

//typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);		// Declare Function Prototype

//PFNGLFOGCOORDFEXTPROC glFogCoordfEXT = NULL;					// Our glFogCoordfEXT Function

#include "opengl/Scene3DPrivate.h"

#else
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------

#include <wrl.h>
#include <d3d11_1.h>
#include <agile.h>
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;

#include "../include/FileUtil.h"
#include "directx/Scene3DPrivate.h"
#include "directx/DXMain.h"
#include "directx/Shader.h"

#endif

#include "../include/Engine.h"

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------

#include "../include/Scene3D.h"

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------

std::vector<Float3> Scene3D::getAll3DPosFrom2D(const Int2& pos)
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	Assert(false);
	return std::vector<Float3>();
#else
	return m_private->getAll3DPosFrom2D(pos, m_camera.getPosition(), m_camera.getMinViewDistance(), m_camera.getMaxViewDistance());
#endif
}


void Scene3D::enableGet3DPosFrom2D(bool yesNo)
{
	m_enablePicking = yesNo;
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
#else
	return m_private->enableGet3DPosFrom2D(yesNo);
#endif
}

Float3 Scene3D::get3DPosFrom2D(const Int2& pos, const void* givenIndicesAndVerticesOrNull)
{
	AssertMessage(m_enablePicking, "You must call enableGet3DPosFrom2D(true) before calling get3DPosFrom2D");
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	//bool is2DMode = !AppSetup::instance().is3DModeInsteadOf2DMode();
	this->set3DMode(false);
	Float3 pos3D = m_private->get3DPosFrom2D(pos);
	Engine::instance().getScene2DMgr().set2DMode();
	return pos3D;
#else
	return m_private->get3DPosFrom2D(pos, m_camera.getPosition(), m_camera.getMinViewDistance(), m_camera.getMaxViewDistance(), givenIndicesAndVerticesOrNull);
#endif
}

void Scene3D::setFogMinDistance(float dist) { m_shader->setFogMinDistance(dist); }
void Scene3D::setFogMaxDistance(float dist) { m_shader->setFogMaxDistance(dist); }
void Scene3D::setFogColor(const Color& parColor) { m_shader->setFogColor(parColor); }
float Scene3D::getFogMinDistance() const { return m_shader->getFogMinDistance(); }
float Scene3D::getFogMaxDistance() const { return m_shader->getFogMaxDistance(); }
const Color& Scene3D::getFogColor() const { return m_shader->getFogColor(); }

void Scene3D::useUserDefinedTextureIdIfAvailable(const std::string& textureFilename)
{
	std::map<std::string, int>::iterator it = m_userDefinedTextureIdFromTextureFile.find(FileUtil::basename(textureFilename));

	int newUserDefinedTextureId = it != m_userDefinedTextureIdFromTextureFile.end() ? (*it).second : 0;
	if (newUserDefinedTextureId != m_shader->getCurrentlySelectedUserDefinedTextureId())
	{
		m_shader->injectUserDefinedTextureId(newUserDefinedTextureId);
		if (m_shader->isUsing())
		{
			//m_shader->stopUsing();
			m_shader->startUsing();
		}
	}
}

void Scene3D::pushPosition(){ m_private->pushPosition(); }
void Scene3D::popPosition(){ m_private->popPosition(); }
void Scene3D::setRenderOppositeFace(bool yesNo) { m_private->setRenderOppositeFace(yesNo); }

void Scene3D::setPosition(const Float3& parNewPosition) { m_private->setPosition(parNewPosition); }
void Scene3D::setPositionAndScaleAndAngle(const Float3& pos, const Float3& scale_, const Float3& angle){ m_private->setPositionAndScaleAndAngle(pos, scale_, angle); }
Float3 Scene3D::getPosition() const { return m_private->getPosition(); }

void Scene3D::setWireframeMode(bool trueForYes) { m_private->setWireframeMode(trueForYes); }

void Scene3D::updateMiscShaderData(float value0, float value1, float value2, bool updateValue0, bool updateValue1, bool updateValue2)
{
	if (updateValue0) m_shader->injectMiscData1(value0);
	if (updateValue1) m_shader->injectMiscData2(value1);
	if (updateValue2) m_shader->injectMiscData3(value2);
	m_shader->startUsing();
}

Scene3D::Scene3D() : m_enablePicking(false)
{
	outputln("Initializing Scene3DMgr...");
	Utils::indentLog();
	m_private = new Scene3DPrivate;
	m_shader = new Shader(m_private->areShadersSupported());

	this->setFogMinDistance(0.f);
	this->setFogMaxDistance(1000.f);
	this->setWireframeMode(false);
	Utils::unindentLog();
	outputln("Initializing Scene3DMgr done.");
}

Scene3D::~Scene3D()
{
	delete m_shader;
	delete m_private;
}

void Scene3D::enableLighting(bool yesNo)
{
	m_shader->enableLight(yesNo);
	m_shader->startUsing();
}

void Scene3D::set3DMode(bool enableLighting)
{
	m_private->set3DMode(this->getCamera());
	m_shader->enableLight(enableLighting);
	m_shader->setAspectRatio(Engine::instance().getScene2DMgr().getWindowAspectRatio());
	m_shader->injectCameraInfos(m_private->getCameraInfos());
	m_shader->startUsing();
}

//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)

// get field of view
float Scene3D::getHorizontalFOV() const
{
	Int2 winSize = Engine::instance().getScene2DMgr().getWindowSize();
	return m_camera.getFieldOfView() * winSize.width() / winSize.height();
}

void Scene3D::adjustFog(float distanceToCameraInWorldCoord, float distanceToCameraInCameraCoord) { m_private->adjustFog(distanceToCameraInWorldCoord, distanceToCameraInCameraCoord); }

bool Scene3D::isFogEnabled() const { return m_private->isFogEnabled(); }

void Scene3D::setFogDisable() { m_private->setFogDisable(); }

void Scene3D::drawCube5FacesWithIntPos(unsigned int textureSidesId, unsigned int textureTopId, unsigned int textureBottomId, const BoundingBoxes::AdvancedBox& box, int depth, bool calculateNormals)
{
	m_private->drawCube5FacesWithIntPos(textureSidesId, textureTopId, textureBottomId, box, depth, calculateNormals);
}

void Scene3D::drawCylinder(unsigned int textureId, float width, float height, int subDiv1, int subDiv2) { m_private->drawCylinder(textureId, width, height, subDiv1, subDiv2); }

void Scene3D::updateCameraSyncronisedWith2D(const Int2& virtualWindowSize) { m_private->updateCameraSyncronisedWith2D(virtualWindowSize, m_camera); }

void Scene3D::configureLight(int idLight, bool enable, const Float3& pos, float parIntensity) { m_private->configureLight(idLight, enable, pos, parIntensity); }

void Scene3D::drawFaceWithIntPos(unsigned int textureId, const BoundingBoxes::AdvancedBox& box, bool mirrorX, bool mirrorY) { m_private->drawFaceWithIntPos(textureId, box, mirrorX, mirrorY); }

void Scene3D::drawCylinderWithIntPos(unsigned int textureId, const Int2& position, float rotationAngle, float width, float height, int subDiv1, int subDiv2)
{
	m_private->drawCylinderWithIntPos(textureId, position, rotationAngle, width, height, subDiv1, subDiv2);
}

void Scene3D::updateLight() { m_private->updateLight(); }

void Scene3D::set3DModeLookAtMode(bool enableLighting, const Float3& parCenter, float distance)
{
	m_private->set3DModeLookAtMode(parCenter, distance, m_camera);

	m_shader->injectCameraInfos(m_private->getCameraInfos());
	m_shader->enableLight(enableLighting);
	m_shader->startUsing();
}

void Scene3D::draw3DSegment(const Float3& parSegmentPt1, const Float3& parSegmentPt2) { m_private->draw3DSegment(parSegmentPt1, parSegmentPt2); }

void Scene3D::startDrawingTriangles(const int textureId) { m_private->startDrawingTriangles(textureId); }

void Scene3D::stopDrawingTriangles() { m_private->stopDrawingTriangles(); }

void Scene3D::drawTriangles(const std::vector<Float3>& parPositionPoints, const int textureId, float scaleFactor) { m_private->drawTriangles(parPositionPoints, textureId, scaleFactor); }

void Scene3D::drawTriangles_requireStartDrawing(const std::vector<Float3>& parPositionPoints, float scaleFactor) { m_private->drawTriangles_requireStartDrawing(parPositionPoints, scaleFactor); }

bool Scene3D::drawCubeFace(const Float3& parSize, unsigned int textureId, const Float3& parTextureRepeat, int faceToDraw, const Float3& cubePosition, bool doOptimizations)
{
	return m_private->drawCubeFace(parSize, textureId, parTextureRepeat, faceToDraw, cubePosition, doOptimizations, this->getHorizontalFOV(), m_camera, this->getFogMaxDistance());
}

void Scene3D::drawCube(const Float3& parSize, const unsigned int parTextureId[6], const Float3& parTextureRepeat, bool reverseToSkyBox)
{
	m_private->drawCube(parSize, parTextureId, parTextureRepeat, reverseToSkyBox);
}

void Scene3D::getVerticesAndTexCoordCube(
	Float3* vertices, Float2* texCoord, const Float3& parTexCoordBegin, const Float3& parTexCoordEnd, const Float3& parTexCoordBeginOtherSide,
	const Float3& parTexCoordEndOtherSide, const Float3& size, bool randomTexCoord)
{
	m_private->getVerticesAndTexCoordCube(vertices, texCoord, parTexCoordBegin, parTexCoordEnd, parTexCoordBeginOtherSide, parTexCoordEndOtherSide, size, randomTexCoord);
}

void Scene3D::drawCube2(const Float3& parSize, const unsigned int parTextureId[6], const Float3& parTextureRepeat, bool reverseToSkyBox, bool alphaTestEffect)
{
	m_private->drawCube2(parSize, parTextureId, parTextureRepeat, reverseToSkyBox, alphaTestEffect);
}

void Scene3D::drawPlane(const Float2& parSize, unsigned int parTextureId, const Float2& parTextureRepeat, const Float3& parAngle, bool reverse)
{
	m_private->drawPlane(parSize, parTextureId, parTextureRepeat, parAngle, reverse);
}

void Scene3D::drawQuad(const Float2& parSize, const Float3 pts[4], unsigned int parTextureId, const Float2& parTextureRepeat)
{
	m_private->drawQuad(parSize, pts, parTextureId, parTextureRepeat);
}

void Scene3D::rotatePosition(float angleX, float angleY, float angleZ) { m_private->rotatePosition(angleX, angleY, angleZ); }

void Scene3D::scalePosition(float x, float y, float z){ m_private->scalePosition(x, y, z); }

void Scene3D::movePosition(const Float3& parMovement){ m_private->movePosition(parMovement); }

unsigned int Scene3D::createAndBeginDrawList() { return m_private->createAndBeginDrawList(); }

void Scene3D::endDrawList(){ m_private->endDrawList(); }

void Scene3D::callDrawList(unsigned int dl){ m_private->callDrawList(dl); }

void Scene3D::removeDrawList(unsigned int dl){ m_private->removeDrawList(dl); }

void Scene3D::startRenderToTexture(float aspectRatio, bool lightingEnable, int parSizeRenderToTexture)
{
	this->set3DMode(lightingEnable);
	Engine::instance().getScene2DMgr().clearScreen(Color(0, 0, 0, 0));
	m_private->startRenderToTexture(aspectRatio, lightingEnable, parSizeRenderToTexture, m_camera);
}

void Scene3D::stopRenderToTexture(unsigned int textureId) { m_private->stopRenderToTexture(textureId); }

Int2 Scene3D::get2DPosFrom3D(const Float3& pos) const { return m_private->get2DPosFrom3D(pos, m_camera); }

int Scene3D::getNbTrianglesDrawn() const { return m_private->getNbTrianglesDrawn(); }

void Scene3D::setMaxVisibleDistance(float parMaxVisibleDistance) { m_private->setMaxVisibleDistance(parMaxVisibleDistance); }

float Scene3D::getMaxVisibleDistance() { return m_private->getMaxVisibleDistance(); }

#else
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------

void Scene3D::clearScene(const Color& backgroundColor)
{
	m_private->clearScene(backgroundColor);
}

//---------------------------------------------------------------------------------------------

void Scene3D::setPositionAndScale(const Float3& pos, const Float3& scale_)
{
	// Update the constant buffer to rotate the cube model.
	m_private->setPositionAndScale(pos, scale_);
}

//---------------------------------------------------------------------------------------------

// This method creates all application resources that depend on
// the application window size.  It is called at app initialization,
// and whenever the application window size changes.
void Scene3D::createWindowSizeDependentResources()
{
	m_private->createWindowSizeDependentResources();
}

//---------------------------------------------------------------------------------------------
/*
void Scene3D::updateCameraLowLevel()
{
m_private->updateCameraLowLevel(this->getCamera());
m_shader->update(Engine::instance().getScene2DMgr().getAspectRatio());
}

void Scene3D::setRenderBlurLevel(int blurLevel)
{
	m_shader->setRenderBlurLevel(blurLevel);
}
*/
//---------------------------------------------------------------------------------------------

Int2 Scene3D::get2DPosFrom3D(const Float3& pos) const {
#pragma message("TODO get2DPosFrom3D")
	return Int2(0, 0);
} // TODO

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

#endif
