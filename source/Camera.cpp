
#include "../include/MathUtils.h"
#include "../include/Utils.h"

#include "../include/Camera.h"

#include "../include/BasicMath.h"

#include "ConstantBuffer.h"

//---------------------------------------------------------------------

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

#define PI_F 3.1415927f

//---------------------------------------------------------------------
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------

Camera::Camera() 
	: m_pos(0.f, 1.0f, 20.0f), m_objPos(0.f, 0.f, 0.f), m_fov(110.f), 
	m_maxView(1000.0f), m_minView(1.f), 
	m_lookAtAngleX(0.f),m_lookAtAngleY(0.f),m_lookAtDistance(100.f)
{}

void Camera::setPosition(const Float3& pos) {m_pos = pos;}

void Camera::move(const Float3& mvt) {m_pos += mvt;}

//---------------------------------------------------------------------

void Camera::setPositionToLookAt(float angleY, float angleX, float distance)
{
	m_lookAtAngleX = angleX;
	m_lookAtAngleY = angleY;
	m_lookAtDistance = distance;
	if (m_lookAtAngleY < -180.f) m_lookAtAngleY += 360.f;
	if (m_lookAtAngleY >  180.f) m_lookAtAngleY -= 360.f;
	if (m_lookAtAngleX >   85.f) m_lookAtAngleX  =  85.f;
	if (m_lookAtAngleX <  -85.f) m_lookAtAngleX  = -85.f;

	float angleXRad = MathUtils::PI *0.5f + m_lookAtAngleX * MathUtils::PI_OVER_180;
	float angleYRad = MathUtils::PI *0.5f - m_lookAtAngleY * MathUtils::PI_OVER_180;

	this->setPosition(m_objPos);
	this->move(Float3(
		m_lookAtDistance * sin(angleXRad) * cos(angleYRad), 
		m_lookAtDistance * cos(angleXRad), 
		m_lookAtDistance * sin(angleXRad) * sin(angleYRad)));
}

//---------------------------------------------------------------------

// this method updates the view matrix based on new position and focus coordinates
static float4x4 getViewParameters(
    _In_ float3 eyePosition,    // the position of the camera
    _In_ float3 lookPosition,   // the point the camera should look at
    _In_ float3 up              // the durection vector for up
    )
{
    float3 m_position = eyePosition;
	float3 m_direction = BasicMath::normalize(lookPosition - eyePosition);
    float3 zAxis = -m_direction;
	float3 xAxis = BasicMath::normalize(BasicMath::cross(up, zAxis));
	float3 yAxis = BasicMath::cross(zAxis, xAxis);
	float xOffset = -BasicMath::dot(xAxis, m_position);
	float yOffset = -BasicMath::dot(yAxis, m_position);
	float zOffset = -BasicMath::dot(zAxis, m_position);
    return float4x4(
        xAxis.x, xAxis.y, xAxis.z, xOffset,
        yAxis.x, yAxis.y, yAxis.z, yOffset,
        zAxis.x, zAxis.y, zAxis.z, zOffset,
        0.0f,    0.0f,    0.0f,    1.0f
        );
}

//---------------------------------------------------------------------

    // this method updates the projection matrix based on new parameters
static float4x4 getProjectionParameters(
    _In_ float minimumFieldOfView,  // the minimum horizontal or vertical field of view, in degrees
    _In_ float aspectRatio,         // the aspect ratio of the projection (width / height)
    _In_ float nearPlane,           // depth to map to 0
    _In_ float farPlane             // depth to map to 1
    )
{
    float minScale = 1.0f / tan(minimumFieldOfView * PI_F / 360.0f);
    float xScale = 1.0f;
    float yScale = 1.0f;
    if (aspectRatio < 1.0f)
    {
        xScale = minScale;
        yScale = minScale * aspectRatio;
    }
    else
    {
        xScale = minScale / aspectRatio;
        yScale = minScale;
    }
    float zScale = farPlane / (farPlane - nearPlane);
    return float4x4(
        xScale, 0.0f,   0.0f,    0.0f,
        0.0f,   yScale, 0.0f,    0.0f,
        0.0f,   0.0f,   -zScale, -nearPlane*zScale,
        0.0f,   0.0f,   -1.0f,   0.0f
        );
}

//---------------------------------------------------------------------

void Camera::updateConstantBuffer(ConstantBuffer* constantBufferData, float windowAspectRatio)
{
	// update the view matrix based on the camera position
    // note that for this sample, the camera position is fixed

	/*float matrix[16];
	this->getMatrix(matrix);
	float* matrix4x4 = (float*)&m_constantBufferData.view;
	memcpy(matrix4x4, matrix, sizeof(float));*/
	
	Float3 camPos = this->getPosition();
	constantBufferData->view = getViewParameters(
		float3(camPos.x(), camPos.y(), camPos.z()),
        float3(m_objPos.x(), m_objPos.y(), m_objPos.z()),
        float3(0, 1, 0));
	
	constantBufferData->projection = getProjectionParameters(
		m_fov,                          // use a 70-degree vertical field of view
        windowAspectRatio,				// specify the aspect ratio of the window
		m_minView,                      // specify the nearest Z-distance at which to draw vertices
        m_maxView                       // specify the farthest Z-distance at which to draw vertices
        );
}

//---------------------------------------------------------------------
/*
void Camera::setLookAt(const Float3& cameraPos, const Float3& objPos)
{
	m_pos = cameraPos;
	m_objPos = objPos;
}
*/

//---------------------------------------------------------------------

const Float3& Camera::getPosition() const
{
	return m_pos;
}

//---------------------------------------------------------------------

Float3 Camera::convertPositionInCameraCoordinates(const Float3& parPositionInWorldCoordinates) const
{
	Utils::die();// not implemented
	/*
	Float3 diff = parPositionInWorldCoordinates - this->getPosition();
	Float4 pos(diff.x(), diff.y(), diff.z(), 0.f);
	return Float3(
		rotMatrix[0]*pos.x()+rotMatrix[4]*pos.y()+rotMatrix[ 8]*pos.z()+rotMatrix[12]*pos.w(),
		rotMatrix[1]*pos.x()+rotMatrix[5]*pos.y()+rotMatrix[ 9]*pos.z()+rotMatrix[13]*pos.w(),
		rotMatrix[2]*pos.x()+rotMatrix[6]*pos.y()+rotMatrix[10]*pos.z()+rotMatrix[14]*pos.w());
		//rotMatrix[3]*pos.x()+rotMatrix[7]*pos.y()+rotMatrix[11]*pos.z()+rotMatrix[15]*pos.w());*/
	return Float3(0.f, 0.f, 0.f);
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
//---------------------------------------------------------------------

#include <GL/glut.h>
#if defined(USES_WINDOWS_OPENGL)
	#include <GL/glext.h>
#endif

//---------------------------------------------------------------------

Camera::Camera()
:m_objPos(0.f, 0.f, 0.f), m_lookAtDistance(100.f), m_fov(110.f), 
m_maxView(1000.0f), m_minView(1.f) //m_maxView(10000.f), m_minView(0.1f), 
{
	this->updateRotationMatrix();
}

//---------------------------------------------------------------------

Camera::~Camera()
{
}

//---------------------------------------------------------------------

void Camera::setPositionToLookAt(float angleY, float angleX, float distance)
{
	if (angleY < -180.f) angleY += 360.f;
	if (angleY >  180.f) angleY -= 360.f;
	if (angleX >   85.f) angleX  =  85.f;
	if (angleX <  -85.f) angleX  = -85.f;

	this->setAngleX(angleX);
	this->setAngleY(angleY);
	m_lookAtDistance = distance;

	float angleXRad = MathUtils::PI *0.5f + angleX * MathUtils::PI_OVER_180;
	float angleYRad = MathUtils::PI *0.5f - angleY * MathUtils::PI_OVER_180;

	this->setPosition(m_objPos);
	this->move(Float3(
		m_lookAtDistance * sin(angleXRad) * cos(angleYRad), 
		m_lookAtDistance * cos(angleXRad), 
		m_lookAtDistance * sin(angleXRad) * sin(angleYRad)));
}

//---------------------------------------------------------------------

//note: the rotation matrix has nothing to do with quaternions
void Camera::updateRotationMatrix()
{
	glPushMatrix();
	glLoadIdentity();
	glRotatef(-getAngleX(), 1.f, 0.f, 0.f);
	glRotatef(-getAngleY(), 0.f, 1.f, 0.f);
	glGetFloatv(GL_MODELVIEW_MATRIX, rotMatrix);
	glPopMatrix();
}

//---------------------------------------------------------------------

Float3 Camera::convertPositionInCameraCoordinates(const Float3& parPositionInWorldCoordinates) const
{
	Float3 diff = parPositionInWorldCoordinates - getPosition();
	Float4 pos(diff.x(), diff.y(), diff.z(), 0.f);
	return Float3(
		rotMatrix[0]*pos.x()+rotMatrix[4]*pos.y()+rotMatrix[ 8]*pos.z()+rotMatrix[12]*pos.w(),
		rotMatrix[1]*pos.x()+rotMatrix[5]*pos.y()+rotMatrix[ 9]*pos.z()+rotMatrix[13]*pos.w(),
		rotMatrix[2]*pos.x()+rotMatrix[6]*pos.y()+rotMatrix[10]*pos.z()+rotMatrix[14]*pos.w());
		//rotMatrix[3]*pos.x()+rotMatrix[7]*pos.y()+rotMatrix[11]*pos.z()+rotMatrix[15]*pos.w());
}

//---------------------------------------------------------------------

#endif
