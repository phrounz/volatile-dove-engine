
#include "WaterSystem.h"

#include "Burning.h"

namespace
{
	const int64_t TIME_ONE_FRAME = 200;//miliseconds
	const double SPEED = 0.1875;

	const int TIME_NEW_FIRE = 500;

	const int ABSOLUTE_NB_MAX_FIRES = 500;

	const int NB_MAX_FIRES_GAME_OVER = 110;	
	const int MAX_FIRES_ON_CANNON = 10;

	const Int2 THERMOMETER_POS(1550, 400);
}

//---------------------------------------------------------------------

Fire::Fire(const Int2& pos, const Int2& posReference):m_animFrame(0), m_pos(pos), m_posReference(posReference)
{
	m_anim.push_back(new Sprite("data/fire1.png"));
	m_anim.push_back(new Sprite("data/fire2.png"));
	m_anim.push_back(new Sprite("data/fire3.png"));
	m_anim.push_back(new Sprite("data/fire4.png"));
	iterateVector(m_anim, Sprite*)
	{
		(*it)->setHotSpotPoint(Int2((int)((*it)->getScaledSize().width()*0.5f), (*it)->getScaledSize().height()));
	}
}

void Fire::manage()
{
	if (Utils::getMillisecond() > m_timeNextFrame)
	{
		m_timeNextFrame = Utils::getMillisecond() + TIME_ONE_FRAME;
		++m_animFrame;
		if (m_animFrame >= 3) m_animFrame = 0;
	}
	m_anim[m_animFrame]->setPosition(m_pos + m_posReference);
}

void Fire::draw() const
{
	m_anim[m_animFrame]->draw();
}

bool Fire::isColliding(const WaterDrop& waterDrop) const
{
	return m_anim[m_animFrame]->isColliding(waterDrop.getSpriteForCollisionTest());
}

bool Fire::isColliding(const Sprite& sprite) const 
{
	return m_anim[m_animFrame]->isColliding(sprite);
}

Fire::~Fire()
{
	iterateVector(m_anim, Sprite*)
	{
		delete *it;
	}
}

//---------------------------------------------------------------------

Burning::Burning():m_timeNextFire(-1)
{
	m_thermometer = new Sprite("data/thermometer.png");
	m_thermometer->setPosition(THERMOMETER_POS);
	
	m_soundBurning = Engine::instance().getSoundMgr().loadSound("data/sounds/47835__tc630__fire2.wav");
}

void Burning::manage(const Int2& posCannon)
{
	if (!m_soundBurning->isPlaying())
	{
		int nbFires = m_fires.size() + m_firesCannon.size() > NB_MAX_FIRES_GAME_OVER ? NB_MAX_FIRES_GAME_OVER : m_fires.size() + m_firesCannon.size();
		m_soundBurning->setVolume(255*nbFires/NB_MAX_FIRES_GAME_OVER);
		m_soundBurning->play();
	}
	
	//----------------
	// other fires

	iterateList(m_fires, Fire*)
	{
		(*it)->manage();
	}
	Autodestroyable::cleanupList(m_fires);
			
	//----------------
	// fires on the cannon

	iterateList(m_firesCannon, Fire*)
	{
		(*it)->setNewPosReference(posCannon);
		(*it)->manage();
	}
	Autodestroyable::cleanupList(m_firesCannon);
}

void Burning::draw() const
{
	//----------------
	// other fires

	iterateListConst(m_fires, Fire*)
	{
		(*it)->draw();
	}
		
	//----------------
	// fires on the cannon

	iterateListConst(m_firesCannon, Fire*)
	{
		(*it)->draw();
	}
	
	//----------------
	// thermometer

	int nbFires = m_fires.size() + m_firesCannon.size() > NB_MAX_FIRES_GAME_OVER ? NB_MAX_FIRES_GAME_OVER : m_fires.size() + m_firesCannon.size();
	Engine::instance().getScene2DMgr().drawRectangle(
		m_thermometer->getPosition()+Int2(13,89-(float)nbFires*1.f*(89.f-5.f)/(float)NB_MAX_FIRES_GAME_OVER),
		m_thermometer->getPosition()+Int2(19,89),
		CoreUtils::colorRed,
		true);
	
	m_thermometer->draw();
}

void Burning::checkCollisions(WaterDrop* waterDrop)
{
	iterateList(m_fires, Fire*)
	{
		if ((*it)->isColliding(*waterDrop))
		{
			(*it)->destroy();
			if (Utils::random(3) == 1) waterDrop->destroy();
		}
	}

	iterateList(m_firesCannon, Fire*)
	{
		if ((*it)->isColliding(*waterDrop))
		{
			(*it)->destroy();
			if (Utils::random(3) == 1) waterDrop->destroy();
		}
	}
}

void Burning::addFire(const Int2& pos)
{
	if (m_fires.size()+m_firesCannon.size() < ABSOLUTE_NB_MAX_FIRES) // avoid eating all computer resources
	{
		m_fires.push_back(new Fire(pos, Int2(0,0)));
	}
}

void Burning::addFireOnCannon(const Int2& pos, const Int2& cannonPos)
{
	if (m_fires.size()+m_firesCannon.size() < ABSOLUTE_NB_MAX_FIRES) // avoid eating all computer resources
	{
		m_firesCannon.push_back(new Fire(pos - cannonPos, cannonPos));
	}
}

bool Burning::hasCannonDamage() const { return m_firesCannon.size() >= MAX_FIRES_ON_CANNON; }

bool Burning::isGameOver() const
{
	return m_fires.size() + m_firesCannon.size() > NB_MAX_FIRES_GAME_OVER;
}

Burning::~Burning()
{
	delete m_thermometer;
	iterateList(m_fires, Fire*)
	{
		delete *it;
	}
	iterateList(m_firesCannon, Fire*)
	{
		delete *it;
	}

	Engine::instance().getSoundMgr().removeSound(m_soundBurning);
}

//---------------------------------------------------------------------
