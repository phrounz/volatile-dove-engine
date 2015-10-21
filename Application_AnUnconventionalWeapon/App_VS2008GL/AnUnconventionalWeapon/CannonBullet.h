#ifndef CannonBullet_h_INCLUDED
#define CannonBullet_h_INCLUDED

#include "include/AbstractMainClass.h"
#include "include/Sprite.h"

#include "Autodestroyable.h"

class Enemy;

//---------------------------------------------------------------------

class CannonBullet: public Autodestroyable
{
public:
	CannonBullet(const Double2& pos, float angleRadians, bool isMini);
	void manage();
	void draw() const 
	{
		m_cannonBullet->draw();
	}
	bool checkCollision(const Enemy& enemy);
	void checkOutOfRange()
	{
		if (m_cannonBulletPos.x() < 0.0) this->autodestroy();
	}
	~CannonBullet();
private:
	Sprite* m_cannonBullet;
	Double2 m_cannonBulletPos;
	Sprite* m_cannonExplosion;
	Double2 m_speed;
};

//---------------------------------------------------------------------

#endif //CannonBullet_h_INCLUDED
