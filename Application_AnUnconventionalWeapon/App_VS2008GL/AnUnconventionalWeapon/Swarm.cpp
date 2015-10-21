
#include "Enemy.h"
#include "EnemyBullet.h"
#include "CannonBullet.h"
#include "Burning.h"
#include "WaterSystem.h"
#include "Cannon.h"

#include "Swarm.h"

namespace
{
	const int64_t TIME_ONE_ENEMY = 1000;//miliseconds
	const int64_t TIME_ONE_LEVEL = 30000;//miliseconds
	const double DIM_TIME_ENEMY_PER_LEVEL = 0.9;//miliseconds
}

//---------------------------------------------------------------------

Swarm::Swarm():m_timeNextEnemy(-1), m_timeNextLevel(-1), m_timeOneEnemy(TIME_ONE_ENEMY), m_level(0), m_nbEnemiesExploded(0), m_nbEnemiesSunk(0), m_nbEnemiesExplodedAndSunk(0)
{
}

void Swarm::manage()//, EngineSystem* engineSystem)
{
	if (Utils::getMillisecond() > m_timeNextEnemy)
	{
		int hw = Engine::instance().getScene2DMgr().getWindowSize().height();
		m_enemies.push_back(new Enemy(Int2(-100, Utils::random(hw))));
		m_timeNextEnemy = Utils::getMillisecond() + m_timeOneEnemy;
	}

	if (Utils::getMillisecond() > m_timeNextLevel)
	{
		m_timeOneEnemy = (int64_t)(m_timeOneEnemy*DIM_TIME_ENEMY_PER_LEVEL);
		m_timeNextLevel = Utils::getMillisecond() + TIME_ONE_LEVEL;
		m_level++;
	}

	iterateList(m_enemies, Enemy*)
	{
		std::vector<EnemyBullet*> newEnemyBullets = (*it)->manage();
		iterateVector2(newEnemyBullets, EnemyBullet*)
		{
			m_enemyBullets.push_back(*it2);
		}
		if ((*it)->isDestroyed() && (*it)->hasSunk()) //note: works only because cleanupList below
		{
			++m_nbEnemiesSunk;
		}
	}
	Autodestroyable::cleanupList(m_enemies);
}

void Swarm::draw() const
{
	iterateListConst(m_enemies, Enemy*)
	{
		(*it)->draw();
	}
	iterateListConst(m_enemyBullets, EnemyBullet*)
	{
		(*it)->draw();
	}
}

void Swarm::checkCollisions(const Image& collisionImage, Burning* burning)
{
	iterateListConst(m_enemyBullets, EnemyBullet*)
	{
		(*it)->manage();
		if ((*it)->isColliding(collisionImage, Int2(0,0)))
		{
			(*it)->destroy();
			burning->addFire((*it)->getPosition());
		}
	}
	Autodestroyable::cleanupList(m_enemyBullets);
}

void Swarm::checkCollisions(Cannon* cannon, Burning* burning)
{
	iterateList(m_enemyBullets, EnemyBullet*)
	{
		if ((*it)->isColliding(cannon->getCollisionImage(), cannon->getCollisionPos()))
		{
			(*it)->destroy();
			burning->addFireOnCannon((*it)->getPosition(), cannon->getCollisionPos());
		}
	}
}

void Swarm::checkCollisions(CannonBullet* cannonBullet)
{
	iterateList(m_enemies, Enemy*)
	{
		if (!(*it)->hasExploded() && cannonBullet->checkCollision(**it))
		{
			if ((*it)->hasSunk())
			{
				++m_nbEnemiesExplodedAndSunk;
			}
			else
			{
				++m_nbEnemiesExploded;
			}
			(*it)->explode();
		}
	}
}

void Swarm::checkCollisions(WaterDrop* waterDrop)
{
	iterateList(m_enemies, Enemy*)
	{
		if (!(*it)->hasExploded() && !(*it)->hasSunk() && waterDrop->checkCollision(*(*it)->getSpriteConst()))
		{
			waterDrop->destroy();
			(*it)->waterImpact();
		}
	}
}

Swarm::~Swarm()
{
	iterateList(m_enemies, Enemy*)
	{
		delete *it;
	}
	iterateList(m_enemyBullets, EnemyBullet*)
	{
		delete *it;
	}
}

//---------------------------------------------------------------------
