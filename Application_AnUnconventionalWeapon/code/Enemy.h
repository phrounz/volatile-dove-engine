#ifndef Enemy_h_INCLUDED
#define Enemy_h_INCLUDED

#include <vector>

#include "../../include/AbstractMainClass.h"
#include "../../include/Sprite.h"
#include "../../include/Utils.h"
#include "../../include/MathUtils.h"

#include "Autodestroyable.h"

class EnemyBullet;

//---------------------------------------------------------------------

class Enemy: public Autodestroyable
{
	
public:
	Enemy(const Int2& pos);
	std::vector<EnemyBullet*> manage();
	void draw() const;
	bool isColliding(const Sprite& sprite) const;
	void onKeyPressed(Key key) {}
	void explode();
	void waterImpact() { ++m_waterDamage; }
	bool hasExploded() const { return m_hasExploded; }
	bool hasSunk() const { return m_hasSunk; }
	~Enemy();
	const Sprite* getSpriteConst() const;
private:
	Sprite* getSprite();
	std::vector<Sprite*> m_anim;
	std::vector<Sprite*> m_animExplosion;
	Sprite* m_sunk;
	Double2 m_pos;
	int m_animFrame;
	int64_t m_timeNextFrame;
	int64_t m_timeNextBullet;
	int64_t m_timeNextDirection;
	Double2 m_currentSpeedVector;
	bool m_hasExploded;
	std::vector<EnemyBullet*> m_tmpBullets;
	int m_waterDamage;
	Sound* m_soundExplode;
	Sound* m_soundSink;
	bool m_hasSunk;
};

//---------------------------------------------------------------------

#endif //Enemy_h_INCLUDED
