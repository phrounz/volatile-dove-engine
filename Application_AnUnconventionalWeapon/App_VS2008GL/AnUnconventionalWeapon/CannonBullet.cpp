
#include "Enemy.h"

#include "CannonBullet.h"

namespace
{
	const double SPEED_BULLET = 0.375;
}

//---------------------------------------------------------------------

CannonBullet::CannonBullet(const Double2& pos, float angleRadians, bool isMini)
:
	m_cannonBulletPos(pos), 
	m_speed(SPEED_BULLET*cos(angleRadians), SPEED_BULLET*sin(angleRadians))
{
	m_cannonBullet = new Sprite(isMini ? "data/cannon_mini_bullet.png" : "data/cannon_bullet.png");
	m_cannonBullet->setHotSpotPointCenter();
	m_cannonBullet->setPosition(Int2(-100,-100));
}

void CannonBullet::manage()
{
	m_cannonBulletPos += m_speed * Engine::instance().getFrameDuration();
	m_cannonBullet->setPosition(Int2((int)m_cannonBulletPos.x(), (int)m_cannonBulletPos.y()));
}

bool CannonBullet::checkCollision(const Enemy& enemy)
{
	bool collide = enemy.isColliding(*m_cannonBullet);
	if (collide) this->autodestroy();
	return collide;
}

CannonBullet::~CannonBullet()
{
	delete m_cannonBullet;
}

//---------------------------------------------------------------------
