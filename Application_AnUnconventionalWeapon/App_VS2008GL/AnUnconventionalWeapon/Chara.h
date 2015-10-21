#ifndef Chara_h_INCLUDED
#define Chara_h_INCLUDED

#include "include/AbstractMainClass.h"
#include "include/Image.h"
#include "include/Bitmap.h"
#include "include/Sprite.h"

#include "Autodestroyable.h"

class WaterSystem;

//---------------------------------------------------------------------

class Bonus: public Autodestroyable
{
public:
	Bonus(const Int2& pos) { m_sprite = new Sprite("data/mini_bullet_counter.png");m_sprite->setScale(Float2(0.8f,0.8f));m_sprite->setHotSpotPointCenter();m_sprite->setPosition(pos); }
	void manage()
	{
		m_sprite->setAngle(m_sprite->getAngle() + Engine::instance().getFrameDuration() * 0.1f);
		while (m_sprite->getAngle() > 360.f) m_sprite->setAngle(m_sprite->getAngle()-360.f);
	}
	void draw() const { m_sprite->draw(); }
	void destroy() { this->autodestroy(); }
	bool isColliding(const Int2& pos) { return m_sprite->getPosition().distanceTo(pos) < 20.f; }
	~Bonus() { delete m_sprite; }
private:
	Sprite* m_sprite;
};

//---------------------------------------------------------------------

class Chara
{
public:

	Chara();
	void manage(Image* collisionImage, WaterSystem* waterSystem);
	void draw() const;
	int popBonuses() { int nbBonuses = m_nbCaughtBonuses;m_nbCaughtBonuses=0;return nbBonuses;}
	~Chara();

private:
	std::vector<Bitmap*> m_anim;
	Double2 m_pos;
	int m_animFrame;
	int64_t m_timeNextFrame;
	bool m_lastDirectionLeft;
	int64_t m_timeNextDrop;
	
	int64_t m_timeNextBonus;
	std::list<Bonus*> m_bonuses;
	int m_nbCaughtBonuses;
	Sound* m_soundBonusPickup;
};

//---------------------------------------------------------------------

#endif //Chara_h_INCLUDED
