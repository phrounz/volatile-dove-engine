#ifndef Burning_h_INCLUDED
#define Burning_h_INCLUDED

#include "include/Utils.h"
#include "include/AbstractMainClass.h"
#include "include/Sprite.h"
#include "include/Image.h"

#include "Autodestroyable.h"

class WaterDrop;

//---------------------------------------------------------------------

class Fire: public Autodestroyable
{
public:
	Fire(const Int2& pos, const Int2& posReference);
	void manage();
	void setNewPosReference(const Int2& pos) { m_posReference = pos; }
	void draw() const;
	bool isColliding(const WaterDrop& waterDrop) const;
	bool isColliding(const Sprite& sprite) const;
	void destroy() { this->autodestroy(); }
	~Fire();
private:
	std::vector<Sprite*> m_anim;
	int m_animFrame;
	int64_t m_timeNextFrame;
	Int2 m_pos;
	Int2 m_posReference;
};

//---------------------------------------------------------------------

class Burning
{
public:

	Burning();
	void manage(const Int2& posCannon);
	void draw() const;
	void checkCollisions(WaterDrop* waterDrop);
	void addFire(const Int2& pos);
	void addFireOnCannon(const Int2& pos, const Int2& cannonPos);
	const std::list<Fire*>& getFires() const { return m_fires; }
	bool hasCannonDamage() const;
	bool isGameOver() const;
	~Burning();

private:
	std::list<Fire*> m_fires;
	std::list<Fire*> m_firesCannon;
	int64_t m_timeNextFire;
	Sprite* m_thermometer;
	Sound* m_soundBurning;
};

//---------------------------------------------------------------------

#endif //Burning_h_INCLUDED
