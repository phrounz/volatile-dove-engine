#ifndef CameraBasic_h_INCLUDED
#define CameraBasic_h_INCLUDED

#include "../../include/CoreUtils.h"

struct CameraPrivate;

//---------------------------------------------------------------------

class CameraBasic
{
public:
	void move(const Float3& mvt);

	void setPosition(const Float3& pos);
	const Float3& getPosition() const;

	//angle are in degrees
	void setRotation(const Float3& rotation);
	void setAngleX(float rotX);
	void setAngleY(float rotY);
	float getAngleX() const;
	float getAngleY() const;
	Float3 getRotation() const;
	void rotateX(float rot);
	void rotateY(float rot);
	void getMatrix(float* matrix) const;
	void getMatrixNotConjugate(float* matrix) const;
protected:
	CameraBasic();
	~CameraBasic();
private:
	struct CameraPrivate* priv;
};

//---------------------------------------------------------------------

#endif //CameraBasic_h_INCLUDED
