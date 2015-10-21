#ifndef Cannon_h_INCLUDED
#define Cannon_h_INCLUDED

#include <vector>

#include "include/AbstractMainClass.h"
#include "include/Sprite.h"
#include "include/Image.h"

#include "Autodestroyable.h"

class Swarm;
class Enemy;
class CannonBullet;
class EngineSystem;
class Fire;
class WaterDrop;

//---------------------------------------------------------------------

class BonusCounter
{
public:
	BonusCounter(const Bitmap& bitmap, const Int2& pos) : m_bitmap(bitmap), m_pos(pos+m_bitmap.size()*0.5f), m_transparency(0.f), m_angle(0.f) {}
	void manage()
	{
		if (m_transparency < 1.f)
		{
			m_transparency += 0.001f*Engine::instance().getFrameDuration();
			if (m_transparency > 1.f) m_transparency = 1.f;
		}
		m_angle += 0.04f*Engine::instance().getFrameDuration();
	}
	void draw() const { m_bitmap.drawRotated(m_pos, m_bitmap.size(), m_angle, m_bitmap.size()*0.5f, false, false, m_transparency); }
	~BonusCounter() {}
private:
	const Bitmap& m_bitmap;
	Int2 m_pos;
	float m_transparency;
	float m_angle;
};

//---------------------------------------------------------------------

class Cannon
{
public:
	
	Cannon();
	void manage(Swarm* swarm, const EngineSystem& engineSystem, bool hasCannonDamage);
	void draw() const;
	void drawBonusesCounter() const;
	void updateBonuses(int nbBonuses) {m_nbMiniBulletShots += nbBonuses;}
	void checkCollisions(WaterDrop* waterDrop);
	const Image& getCollisionImage() const { return *m_cannonCollisionImage; }
	Int2 getCollisionPos() const { return CoreUtils::fromDouble2ToInt2(m_pos)-m_cannonCollisionImage->size()*0.5f; } // top-left sprite pos
	~Cannon();

private:
	std::vector<Sprite*> m_anim;
	Double2 m_pos;
	int64_t m_timeNextFire;
	int m_animFrame;
	int64_t m_timeNextFrame;
	bool m_animFinished;
	std::list<CannonBullet*> m_cannonBullets;
	int m_nbMiniBulletShots;
	Bitmap* m_bonusCounterBitmap;
	Bitmap* m_cannonBulletForMachine;
	double m_distBulletMachineUp;
	Sound* m_soundFire;
	Sound* m_soundFireMiniBullets;
	Image* m_cannonCollisionImage;
	std::vector<BonusCounter*> m_bonusCounters;
	Sprite* m_cannonDamaged;
	bool m_hasCannonDamage;
};

//---------------------------------------------------------------------

#endif //Cannon_h_INCLUDED
