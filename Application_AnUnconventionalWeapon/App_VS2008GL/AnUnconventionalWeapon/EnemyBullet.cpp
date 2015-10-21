
#include "EnemyBullet.h"

EnemyBullet::EnemyBullet(const Double2& pos, const Double2& speed)
:m_speed(speed), m_pos(pos)
{
	m_sprite = new Sprite("data/enemy_bullet.png");
	m_sprite->setHotSpotPointCenter();
}

void EnemyBullet::manage()
{
	m_pos += m_speed * (float)Engine::instance().getFrameDuration();
	m_sprite->setPosition(CoreUtils::fromDouble2ToInt2(m_pos));

	Int2 winSize = Engine::instance().getScene2DMgr().getWindowSize();
	if (m_pos.x() < 0.0 || m_pos.y() < 0.0 || m_pos.x() >= winSize.width() || m_pos.y() >= winSize.height())
	{
		this->autodestroy();
	}
}

bool EnemyBullet::isColliding(const Image& collisionImage, const Int2& collisionImagePos) const
{
	Int2 winSize = Engine::instance().getScene2DMgr().getWindowSize();
	if (!(m_pos.x() < collisionImagePos.x() || m_pos.y() < collisionImagePos.y() 
		|| m_pos.x() >= collisionImagePos.x() + collisionImage.size().width() || m_pos.y() >= collisionImagePos.y() + collisionImage.size().height()))
	{
		if (collisionImage.getPixel(CoreUtils::fromDouble2ToInt2(m_pos) - collisionImagePos).a() != 0)
		{
			return true;
		}
	}
	return false;
}

void EnemyBullet::draw() const
{
	m_sprite->draw();
}

EnemyBullet::~EnemyBullet()
{
	delete m_sprite;
}
