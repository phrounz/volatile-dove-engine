/******************************************************************
Done by Francois Braud
******************************************************************/
#ifndef Camera_h_INCLUDED
#define Camera_h_INCLUDED

#include "CoreUtils.h"

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	#include "../source/opengl/CameraBasic.h"
#endif

struct ConstantBuffer;
class Scene3DPrivate;
class Scene3D;

//---------------------------------------------------------------------

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
class Camera
#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
class Camera: public CameraBasic
#endif
{
	friend class Scene3DPrivate;
	friend class Scene3D;
public:
	void setMinViewDistance(float minView) {m_minView = minView;}
	void setMaxViewDistance(float maxView) {m_maxView = maxView;}
	inline float getMinViewDistance() const {return m_minView;}
	inline float getMaxViewDistance() const {return m_maxView;}

	float getDistance() const {return m_lookAtDistance;}

	void setFieldOfView(float fov) {m_fov = fov;}
	float getFieldOfView() const {return m_fov;}

	void setLookAt(const Float3& objPos) {m_objPos = objPos;}//what the camera looks
	const Float3& getLookAt() const {return m_objPos;}

	void setPositionToLookAt(float angleY, float angleX, float distance);//the position of the camera which looks at objPos
	
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	void setAngleX(float angle) {} // TODO
	void setAngleY(float angle) {} // TODO
#endif
	
private:
	float m_minView;
	float m_maxView;

	float m_lookAtDistance;

	float m_fov;

	Float3 m_objPos;

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO) //------------------------------------
public:
	
	void setPosition(float x, float y, float z) { this->setPosition(Float3(x,y,z));}
	void setPosition(const Float3& pos);
	const Float3& getPosition() const;

public:
	Camera();

	float getAngleX() const {return m_lookAtAngleX;}
	float getAngleY() const {return m_lookAtAngleY;}

	void move(const Float3& mvt);

	Float3 convertPositionInCameraCoordinates(const Float3& parPositionInWorldCoordinates) const;
private:
	void updateConstantBuffer(ConstantBuffer* constantBufferData, float windowAspectRatio);
	Float3 m_pos;

	float m_lookAtAngleX;
	float m_lookAtAngleY;

#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) //------------------------------------

public:
	
	Float3 convertPositionInCameraCoordinates(const Float3& parPositionInWorldCoordinates) const;

private:
	Camera();
	~Camera();
	void rotateX(float rotX) { CameraBasic::rotateX(rotX);this->updateRotationMatrix();}
	void rotateY(float rotY) { CameraBasic::rotateX(rotY);this->updateRotationMatrix();}
	void updateRotationMatrix();
	
	float rotMatrix[16];

#endif //------------------------------------
};

//---------------------------------------------------------------------

#endif //Camera_h_INCLUDED
