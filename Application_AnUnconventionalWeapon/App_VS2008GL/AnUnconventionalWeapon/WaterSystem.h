#ifndef WaterSystem_h_INCLUDED
#define WaterSystem_h_INCLUDED

#include "include/Engine.h"
#include "include/Image.h"
#include "include/Bitmap.h"
#include "include/Sprite.h"

#include "Autodestroyable.h"

//---------------------------------------------------------------------

class WaterDrop: public Autodestroyable
{
public:
	WaterDrop(const Int2& pos, const Double2& speed) : m_speed(speed), m_pos(CoreUtils::fromInt2ToDouble2(pos))
	{ 
		m_sprite = new Sprite("data/water_drop.png");
		m_sprite->setHotSpotPointCenter();
	}
	void manage()
	{
		m_pos += m_speed * (float)(Engine::instance().getFrameDuration() * 0.1f);
		m_sprite->setPosition(CoreUtils::fromDouble2ToInt2(m_pos));

		m_speed.data[1] += 0.1 * (double)Engine::instance().getFrameDuration() * 0.1;

		if (m_pos.y() > Engine::instance().getScene2DMgr().getWindowSize().height()) this->autodestroy();
	}
	void draw() const { m_sprite->draw(); }
	const Sprite& getSpriteForCollisionTest() const { return *m_sprite; }
	bool checkCollision(const Sprite& sprite) const { return m_sprite->isColliding(sprite); }
	void destroy() { this->autodestroy(); }
	~WaterDrop() { delete m_sprite; }
private:
	Sprite* m_sprite;
	Double2 m_pos;
	Double2 m_speed;
};

//---------------------------------------------------------------------

class WaterSystem
{
public:
	WaterSystem()
	{
	}

	std::list<WaterDrop*>& manage()
	{
		iterateListConst(m_waterDrops, WaterDrop*)
		{
			(*it)->manage();
		}
		Autodestroyable::cleanupList(m_waterDrops);

		return m_waterDrops;
	}

	void draw() const
	{
		iterateListConst(m_waterDrops, WaterDrop*)
		{
			(*it)->draw();
		}
	}

	void addDrop(const Int2& pos, const Double2& speed)
	{
		m_waterDrops.push_back(new WaterDrop(pos, speed));
	}

	~WaterSystem()
	{
		iterateList(m_waterDrops, WaterDrop*)
		{
			delete *it;
		}
	}
private:
	std::list<WaterDrop*> m_waterDrops;
};

//---------------------------------------------------------------------

#endif //WaterSystem_h_INCLUDED

