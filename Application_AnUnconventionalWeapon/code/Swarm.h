#ifndef Swarm_h_INCLUDED
#define Swarm_h_INCLUDED

#include <list>

#include "../../include/CoreUtils.h"
#include "../../include/Utils.h"
#include "../../include/AbstractMainClass.h"
#include "../../include/Sprite.h"
#include "../../include/Image.h"

class Enemy;
class EnemyBullet;
class CannonBullet;
class Burning;
class EngineSystem;
class WaterDrop;
class Cannon;

class Swarm
{
public:

	Swarm();
	void manage();//, EngineSystem* engineSystem);
	void draw() const;
	void checkCollisions(const Image& collisionImage, Burning* burning);
	void checkCollisions(Cannon* cannon, Burning* burning);
	void checkCollisions(CannonBullet* cannonBullet);
	void checkCollisions(WaterDrop* waterDrop);
	~Swarm();

	int getNbEnemiesExploded() const { return m_nbEnemiesExploded; }
	int getNbEnemiesSunk() const { return m_nbEnemiesSunk; }
	int getNbEnemiesExplodedAndSunk() const { return m_nbEnemiesExplodedAndSunk; }
	int getLevel() const { return m_level; }

private:
	std::list<Enemy*> m_enemies;
	std::list<EnemyBullet*> m_enemyBullets;
	int64_t m_timeNextEnemy;
	int64_t m_timeNextLevel;
	int64_t m_timeOneEnemy;
	int m_level;
	int m_nbEnemiesExploded;
	int m_nbEnemiesSunk;
	int m_nbEnemiesExplodedAndSunk;
};

#endif //Swarm_h_INCLUDED
