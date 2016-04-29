
#include "EnemyBullet.h"

#include "Enemy.h"

namespace
{
	const int64_t TIME_ONE_FRAME = 200;//miliseconds
	const int64_t TIME_ONE_BULLET = 1000;
	const int64_t TIME_ONE_DIRECTION = 1000;//miliseconds
	const double SPEED = 0.125;
	const double SPEED_BULLET = 0.3125;
	const int MAX_WATER_DAMAGE = 10;
	const Double2 SPEED_SUNK(0.1, 0.1);
}

//---------------------------------------------------------------------

Enemy::Enemy(const Int2& pos)
: 
	m_pos(CoreUtils::fromInt2ToDouble2(pos)), 
	m_animFrame(0), 
	m_hasExploded(false), 
	m_timeNextFrame(-1), 
	m_timeNextBullet(-1), 
	m_timeNextDirection(-1), 
	m_currentSpeedVector(SPEED, 0.0), 
	m_waterDamage(0),
	m_soundExplode(NULL),
	m_hasSunk(false)
{
	m_anim.push_back(new Sprite("data/enemy1.png"));
	m_anim.push_back(new Sprite("data/enemy2.png"));
	m_anim.push_back(new Sprite("data/enemy3.png"));
	m_anim.push_back(new Sprite("data/enemy4.png"));
	iterateVector(m_anim, Sprite*)
	{
		(*it)->setHotSpotPointCenter();
	}

	m_animExplosion.push_back(new Sprite("data/enemy_explosion1.png"));
	m_animExplosion.push_back(new Sprite("data/enemy_explosion2.png"));
	m_animExplosion.push_back(new Sprite("data/enemy_explosion3.png"));
	m_animExplosion.push_back(new Sprite("data/enemy_explosion4.png"));
	iterateVector(m_animExplosion, Sprite*)
	{
		(*it)->setHotSpotPointCenter();
	}

	m_sunk = new Sprite("data/enemy_sunk.png");
	m_sunk->setHotSpotPointCenter();

	m_soundExplode = Engine::instance().getSoundMgr().loadSound("data/sounds/182429__qubodup__explosion__mod.wav");
	m_soundSink = Engine::instance().getSoundMgr().loadSound("data/sounds/155332__bradduhig__plane-crashing__mod.wav");
}

void Enemy::explode()
{
	if (m_hasSunk) { m_hasSunk = false; m_waterDamage = 0; }
	m_hasExploded = true;
	m_animFrame = 0;
	m_soundExplode->play();
}

std::vector<EnemyBullet*> Enemy::manage()
{
	if (!this->hasSunk() && m_waterDamage > MAX_WATER_DAMAGE)
	{
		m_hasSunk = true;
		m_soundSink->play();
	}

	if (this->hasSunk())
	{
		m_pos += SPEED_SUNK * (float)Engine::instance().getFrameDuration();
	}
	else if (!this->hasExploded())
	{
		m_pos += m_currentSpeedVector * (float)Engine::instance().getFrameDuration();
		if (Utils::getMillisecond() > m_timeNextDirection)
		{
			m_timeNextDirection = Utils::getMillisecond() + TIME_ONE_DIRECTION;

			float angle = -MathUtils::PI / 2.f + Utils::random(MathUtils::PI);
			m_currentSpeedVector = Double2(SPEED*cos(angle), SPEED*sin(angle));

		}
		KeyboardManager& keyboardMgr = Engine::instance().getKeyboardMgr();
		int randomDirection = Utils::random(4);

		double mvt =  SPEED * Engine::instance().getFrameDuration();
		if (randomDirection == 0)
		{
			m_pos.data[1] -= mvt;
		}
		else if (randomDirection == 1)
		{
			m_pos.data[1] += mvt;
		}
		else if (randomDirection == 2)
		{
			m_pos.data[0] += mvt;
		}
		else //3
		{
			m_pos.data[0] += mvt;
		}
	}

	if (!this->hasSunk())
	{
		if (Utils::getMillisecond() > m_timeNextFrame)
		{
			m_timeNextFrame = Utils::getMillisecond() + TIME_ONE_FRAME;
			++m_animFrame;
			if (m_animFrame >= 3)
			{
				m_animFrame = 0;
				if (m_hasExploded)
				{
					this->autodestroy();
				}
			}
		}

		if (!this->hasExploded())
		{
			if (Utils::getMillisecond() > m_timeNextBullet)
			{
				m_timeNextBullet = Utils::getMillisecond() + TIME_ONE_BULLET;

				float angle = -MathUtils::PI / 4.f + Utils::random(MathUtils::PI / 2.f);
				Double2 bulletSpeedVector(SPEED_BULLET*cos(angle), SPEED_BULLET*sin(angle));
				m_tmpBullets.push_back(new EnemyBullet(m_pos, bulletSpeedVector));
			}
		}
	}

	this->getSprite()->setPosition(CoreUtils::fromDouble2ToInt2(m_pos));

	if ((int)m_pos.x() >= Engine::instance().getScene2DMgr().getWindowSize().width()+m_anim[m_animFrame]->getScaledSize().width())
	{
		this->autodestroy();
	}
	else if ((int)m_pos.y() >= Engine::instance().getScene2DMgr().getWindowSize().height()+m_anim[m_animFrame]->getScaledSize().height())
	{
		this->autodestroy();
	}

	std::vector<EnemyBullet*> bulletsCopy = m_tmpBullets;
	m_tmpBullets.clear();
	return bulletsCopy;
}

void Enemy::draw() const
{
	this->getSpriteConst()->draw();
}

Sprite* Enemy::getSprite()
{
	if (this->hasSunk())
		return m_sunk;
	else if (this->hasExploded())
		return m_animExplosion[m_animFrame];
	else
		return m_anim[m_animFrame];
}

const Sprite* Enemy::getSpriteConst() const
{
	if (this->hasSunk())
		return m_sunk;
	else if (this->hasExploded())
		return m_animExplosion[m_animFrame];
	else
		return m_anim[m_animFrame];
}

bool Enemy::isColliding(const Sprite& sprite) const
{
	return this->getSpriteConst()->isColliding(sprite);
}

Enemy::~Enemy()
{
	Engine::instance().getSoundMgr().removeSound(m_soundExplode);
	Engine::instance().getSoundMgr().removeSound(m_soundSink);

	iterateVector(m_anim, Sprite*)
	{
		delete *it;
	}

	iterateVector(m_animExplosion, Sprite*)
	{
		delete *it;
	}

	delete m_sunk;
}

//---------------------------------------------------------------------
