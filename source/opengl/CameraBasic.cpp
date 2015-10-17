
#include "CameraBasic.h"

//---------------------------------------------------------------------

//http://gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation

class Quaternion
{
public:
	float x,y,z,w;
	
	Quaternion(float parX,float parY, float parZ, float parW)
	{
		x = parX;
		y = parY;
		z = parZ;
		w = parW;
	}

	Quaternion()
	{}

	Quaternion(const Float3 &v, float angle)
	{
		float sinAngle;
		angle *= 0.5f;
		Float3 vn(v);
		vn.normalize();
	 
		sinAngle = sin(angle);
	 
		x = (vn.data[0] * sinAngle);
		y = (vn.data[1] * sinAngle);
		z = (vn.data[2] * sinAngle);
		w = cos(angle);
	}


	void normalise()
	{
		// Don't normalize if we don't have to
		float mag2 = w * w + x * x + y * y + z * z;
		//if (fabs(mag2 - 1.0f) > TOLERANCE) {
			float mag = sqrt(mag2);
			w /= mag;
			x /= mag;
			y /= mag;
			z /= mag;
		//}
	}
	Quaternion getConjugate() const
	{
		return Quaternion(-x, -y, -z, w);
	}
	
	Quaternion operator* (const Quaternion &rq) const
	{
		return Quaternion(w * rq.x + x * rq.w + y * rq.z - z * rq.y,
						  w * rq.y + y * rq.w + z * rq.x - x * rq.z,
						  w * rq.z + z * rq.w + x * rq.y - y * rq.x,
						  w * rq.w - x * rq.x - y * rq.y - z * rq.z);
	}
	Float3 operator* (const Float3 &vec) const
	{
		Float3 vn(vec);
		vn.normalize();
	 
		Quaternion vecQuat, resQuat;
		vecQuat.x = vn.x();
		vecQuat.y = vn.y();
		vecQuat.z = vn.z();
		vecQuat.w = 0.0f;
	 
		resQuat = vecQuat * getConjugate();
		resQuat = *this * resQuat;
	 
		return Float3(resQuat.x, resQuat.y, resQuat.z);
	}
	void getAxisAngle(Float3* axis, float *angle)
	{
		float scale = sqrt(x * x + y * y + z * z);
		axis->data[0] = x / scale;
		axis->data[1] = y / scale;
		axis->data[2] = z / scale;
		*angle = acos(w) * 2.0f;
	}
	void FromEuler(float pitch, float yaw, float roll)
	{
		float p = pitch * MathUtils::PI_OVER_180 / 2.f;
		float y = yaw * MathUtils::PI_OVER_180 / 2.f;
		float r = roll * MathUtils::PI_OVER_180 / 2.f;
	 
		float sinp = sin(p);
		float siny = sin(y);
		float sinr = sin(r);
		float cosp = cos(p);
		float cosy = cos(y);
		float cosr = cos(r);
	 
		this->x = sinr * cosp * cosy - cosr * sinp * siny;
		this->y = cosr * sinp * cosy + sinr * cosp * siny;
		this->z = cosr * cosp * siny - sinr * sinp * cosy;
		this->w = cosr * cosp * cosy + sinr * sinp * siny;
	 
		normalise();
	}
	float* getNewMatrix() const
	{
		float* matrix = new float[16];
		getMatrix(matrix);
		return matrix;
	}
	void getMatrix(float* matrix) const
	{
		if(!matrix) return;
		matrix[0]  = 1.0f - 2.0f * ( y * y + z * z );
		matrix[1]  = 2.0f * (x * y + z * w);
		matrix[2]  = 2.0f * (x * z - y * w);
		matrix[3]  = 0.0f;
		matrix[4]  = 2.0f * ( x * y - z * w );
		matrix[5]  = 1.0f - 2.0f * ( x * x + z * z );
		matrix[6]  = 2.0f * (z * y + x * w );
		matrix[7]  = 0.0f;
		matrix[8]  = 2.0f * ( x * z + y * w );
		matrix[9]  = 2.0f * ( y * z - x * w );
		matrix[10] = 1.0f - 2.0f * ( x * x + y * y );
		matrix[11] = 0.0f;
		matrix[12] = 0;
		matrix[13] = 0;
		matrix[14] = 0;
		matrix[15] = 1.0f;
	}
};

//---------------------------------------------------------------------

struct CameraPrivate
{
	Float3 pos;
	Quaternion rotation;
	float angleX;
	float angleY;

};

//---------------------------------------------------------------------

CameraBasic::CameraBasic()
{
	priv = new CameraPrivate;
	priv->pos = Float3(0,0,0);
	priv->rotation = Quaternion(Float3(1.f, 0.f, 0.f), 0.f);
	priv->angleX = 0.f;
	priv->angleY = 0.f;
}

CameraBasic::~CameraBasic()
{
	delete priv;
}

void CameraBasic::move(const Float3& mvt)
{
	priv->pos += mvt;
	/*priv->pos += priv->rotation * Float3(mvt.x(), 0.0f, 0.0f);
	priv->pos.data[1] -= mvt.y();
	priv->pos += priv->rotation * Float3(0.0f, 0.0f, -mvt.z());*/
}

void CameraBasic::setPosition(const Float3& pos)
{
	priv->pos = pos;
}

const Float3& CameraBasic::getPosition() const
{
	return priv->pos;
}

void CameraBasic::setRotation(const Float3& rot)
{
	priv->rotation = Quaternion(Float3(1.f, 0.f, 0.f), 0.f);
	priv->angleX = 0.f;
	priv->angleY = 0.f;
	rotateX(rot.x());
	rotateY(rot.y());
	Assert(rot.z() == 0.f);
}

void CameraBasic::setAngleX(float rotX)
{
	rotateX(-priv->angleX);
	priv->angleX = 0.f;
	rotateX(rotX);
}

void CameraBasic::setAngleY(float rotY)
{
	rotateY(-priv->angleY);
	priv->angleY = 0.f;
	rotateY(rotY);
}

float CameraBasic::getAngleX() const
{
	return priv->angleX;
}

float CameraBasic::getAngleY() const
{
	return priv->angleY;
}

void CameraBasic::rotateX(float rotX)
{
	Quaternion nrot1(Float3(1.0f, 0.0f, 0.0f), rotX * MathUtils::PI_OVER_180);
	priv->rotation = priv->rotation * nrot1;
	priv->angleX += rotX;
	//while (priv->angleX >= 180.f) priv->angleX -= 360.f;
	//while (priv->angleX < -180.f) priv->angleX += 360.f;
}

void CameraBasic::rotateY(float rotY)
{
	Quaternion nrot2(Float3(0.0f, 1.0f, 0.0f), rotY * MathUtils::PI_OVER_180);
	priv->rotation = nrot2 * priv->rotation;
	priv->angleY += rotY;
	//while (priv->angleY >= 180.f) priv->angleY -= 360.f;
	//while (priv->angleY < -180.f) priv->angleY += 360.f;
}

Float3 CameraBasic::getRotation() const
{
	return Float3(priv->angleX, priv->angleY, 0.f);
}

void CameraBasic::getMatrix(float* matrix) const
{
	priv->rotation.getConjugate().getMatrix(matrix);
}

void CameraBasic::getMatrixNotConjugate(float* matrix) const
{
	priv->rotation.getMatrix(matrix);
}

//---------------------------------------------------------------------
