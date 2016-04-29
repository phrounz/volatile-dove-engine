#ifndef Cloud_h_INCLUDED
#define Cloud_h_INCLUDED

#include "../../include/CoreUtils.h"
#include "../../include/Utils.h"
#include "../../include/AbstractMainClass.h"
#include "../../include/Sprite.h"

namespace
{
	double CLOUD_SPEED_MAX = 0.1875;
	double CLOUD_SPEED_MIN = 0.0625;
}

//---------------------------------------------------------------------

class Cloud
{
public:
	Cloud() : m_pos(0.0,0.0), m_speed(0.0)
	{
		m_sprite = new Sprite("data/cloud.png");
		m_sprite->setHotSpotPointCenter();
		m_pos.data[0] = -10000.0;
		m_pos.data[1] = 0.0;
	}

	void manage()
	{
		m_pos.data[0] -= m_speed * Engine::instance().getFrameDuration();
		if (m_pos.data[0] < -m_sprite->getScaledSize().width()*0.5f)
		{
			m_sprite->setScale(Float2(Utils::random(1.2f)+0.5f));
			m_sprite->setHotSpotPointCenter();
			m_pos.data[0] = Engine::instance().getScene2DMgr().getWindowSize().width() + m_sprite->getScaledSize().width()*0.5f;
			m_pos.data[1] = (double)Utils::random(Engine::instance().getScene2DMgr().getWindowSize().height());
			m_speed = (Utils::random(CLOUD_SPEED_MAX-CLOUD_SPEED_MIN)+CLOUD_SPEED_MIN);
		}
		m_sprite->setPosition(Int2((int)m_pos.x(), (int)m_pos.y()));
	}

	void draw() const { m_sprite->draw(); }

	~Cloud()
	{
		delete m_sprite;
	}

private:
	Sprite* m_sprite;
	Double2 m_pos;
	double m_speed;
};

//---------------------------------------------------------------------

#endif //Cloud_h_INCLUDED
