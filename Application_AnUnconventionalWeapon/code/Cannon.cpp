
#include "Swarm.h"
#include "CannonBullet.h"
#include "EngineSystem.h"
#include "Burning.h"
#include "WaterSystem.h"

#include "Cannon.h"

namespace
{
	const int64_t TIME_ONE_FRAME = 100;//miliseconds
	const double SPEED = 0.375;

	const int64_t TIME_ONE_FIRE = 500;

	const int NB_BONUSES_BEGINNING = 2;
}

//---------------------------------------------------------------------

Cannon::Cannon()
: m_pos(700.0,100.0), m_animFrame(0), m_animFinished(true),m_timeNextFire(-1), m_nbMiniBulletShots(NB_BONUSES_BEGINNING), m_distBulletMachineUp(0.0), m_hasCannonDamage(false)
{
	m_anim.push_back(new Sprite("data/cannon1.png"));
	m_anim.push_back(new Sprite("data/cannon2.png"));
	m_anim.push_back(new Sprite("data/cannon3.png"));
	m_anim.push_back(new Sprite("data/cannon4.png"));
	m_anim.push_back(new Sprite("data/cannon5.png"));
	m_anim.push_back(new Sprite("data/cannon6.png"));
	m_anim.push_back(new Sprite("data/cannon7.png"));
	iterateVector(m_anim, Sprite*)
	{
		(*it)->setHotSpotPointCenter();
	}
	
	m_cannonDamaged = new Sprite("data/cannon_damaged.png");
	m_cannonDamaged->setHotSpotPointCenter();
	m_cannonBulletForMachine = new Bitmap("data/cannon_bullet.png");
	m_cannonCollisionImage = new Image("data/cannon_collision.png");

	m_soundFire = Engine::instance().getSoundMgr().loadSound("data/sounds/91293__baefild__uncompressed-cannon__mod.wav");
	m_soundFireMiniBullets = Engine::instance().getSoundMgr().loadSound("data/sounds/25589__daveincamas__20061111nailgun__mod.wav");

	m_bonusCounterBitmap = new Bitmap("data/mini_bullet_counter.png");

	Int2 posInt2 = CoreUtils::fromDouble2ToInt2(m_pos);
	m_anim[m_animFrame]->setPosition(posInt2);
	m_cannonDamaged->setPosition(posInt2);
}

void Cannon::manage(Swarm* swarm, const EngineSystem& engineSystem, bool hasCannonDamage)
{
	m_hasCannonDamage = hasCannonDamage;

	bool keyPressed = false;
	
	//----------------
	// movement

	KeyboardManager& keyboardMgr = Engine::instance().getKeyboardMgr();
	JoystickManager& joystickMgr = Engine::instance().getJoystickMgr();
	if ((keyboardMgr.getKeyNoCase('u') || joystickMgr.isPressed(JoystickManager::PLAYER_ONE, JoystickManager::BUTTON_UP) 
		|| joystickMgr.isPressed(JoystickManager::PLAYER_ONE, JoystickManager::LEFT_STICK_UP_AS_BUTTON))
		&& m_pos.y() > 0)
	{
		m_pos.data[1] -= SPEED * Engine::instance().getFrameDuration();
		keyPressed = true;
	}
	else if ((keyboardMgr.getKeyNoCase('j') || joystickMgr.isPressed(JoystickManager::PLAYER_ONE, JoystickManager::BUTTON_DOWN)
		|| joystickMgr.isPressed(JoystickManager::PLAYER_ONE, JoystickManager::LEFT_STICK_DOWN_AS_BUTTON))
		&& m_pos.y() < Engine::instance().getScene2DMgr().getWindowSize().height())
	{
		m_pos.data[1] += SPEED * Engine::instance().getFrameDuration();
		keyPressed = true;
	}

	if (Utils::getMillisecond() > m_timeNextFrame && !m_animFinished)
	{
		m_timeNextFrame = Utils::getMillisecond() + TIME_ONE_FRAME;
		++m_animFrame;
		if (m_animFrame >= 7) {m_animFrame = 0;m_animFinished = true;}
	}

	Int2 posInt2 = CoreUtils::fromDouble2ToInt2(m_pos);

	//----------------
	// cannon bullets

	iterateList(m_cannonBullets, CannonBullet*)
	{
		(*it)->manage();
		(*it)->checkOutOfRange();
		swarm->checkCollisions(*it);
	}
	Autodestroyable::cleanupList(m_cannonBullets);
	
	m_anim[m_animFrame]->setPosition(posInt2);
	m_cannonDamaged->setPosition(posInt2);

	//----------------
	// shots

	if ((keyboardMgr.getKeyNoCase('s') || joystickMgr.isPressed(JoystickManager::PLAYER_ONE, JoystickManager::BUTTON_X)) 
		&& Utils::getMillisecond() > m_timeNextFire && !m_hasCannonDamage)
	{
		int64_t timeOneFire = TIME_ONE_FIRE;
		if (engineSystem.hasLatencyMachineBigDamage()) timeOneFire *= 4;
		else if (engineSystem.hasLatencyMachineSmallDamage()) timeOneFire *= 2;

		m_timeNextFire = Utils::getMillisecond() + timeOneFire;
		m_animFinished = false;

		float angle = -MathUtils::PI;
		if (engineSystem.hasPrecisionMachineBigDamage())
			angle = -MathUtils::PI - MathUtils::PI/4.f + Utils::random(MathUtils::PI/2.f);
		else if (engineSystem.hasPrecisionMachineSmallDamage())
			angle = -MathUtils::PI - MathUtils::PI/8.f + Utils::random(MathUtils::PI/4.f);
		
		m_cannonBullets.push_back(new CannonBullet(m_pos, angle, false));

		m_soundFire->play();

		m_distBulletMachineUp = 0.0;
	}

	if ((keyboardMgr.getKeyNoCase('d') || joystickMgr.isPressed(JoystickManager::PLAYER_ONE, JoystickManager::BUTTON_Y))
		&& Utils::getMillisecond() > m_timeNextFire && m_nbMiniBulletShots && !m_hasCannonDamage)
	{
		m_nbMiniBulletShots--;
		int64_t timeOneFire = TIME_ONE_FIRE;
		if (engineSystem.hasLatencyMachineBigDamage()) timeOneFire *= 4;
		else if (engineSystem.hasLatencyMachineSmallDamage()) timeOneFire *= 2;

		m_timeNextFire = Utils::getMillisecond() + timeOneFire;
		m_animFinished = false;

		float angle = -MathUtils::PI;
		m_cannonBullets.push_back(new CannonBullet(m_pos, angle - 0.2f*MathUtils::PI, true));
		m_cannonBullets.push_back(new CannonBullet(m_pos, angle - 0.1f*MathUtils::PI, true));
		m_cannonBullets.push_back(new CannonBullet(m_pos, angle, true));
		m_cannonBullets.push_back(new CannonBullet(m_pos, angle + 0.2f*MathUtils::PI, true));
		m_cannonBullets.push_back(new CannonBullet(m_pos, angle + 0.1f*MathUtils::PI, true));

		m_soundFireMiniBullets->play();

		m_distBulletMachineUp = 0.0;
	}

	if (m_distBulletMachineUp > -m_cannonBulletForMachine->size().height())
	{
		 m_distBulletMachineUp -= 0.2 * Engine::instance().getFrameDuration();
	}
	else
	{
		m_distBulletMachineUp = -m_cannonBulletForMachine->size().height();
	}

	//----------------
	// bonus counters

	iterateVectorConst(m_bonusCounters, BonusCounter*)
	{
		(*it)->manage();
	}

	if ((int)m_bonusCounters.size() < m_nbMiniBulletShots)
	{
		for (int i = m_bonusCounters.size(); i < m_nbMiniBulletShots; ++i)
		{
			m_bonusCounters.push_back(new BonusCounter(*m_bonusCounterBitmap, Int2(10+(m_bonusCounterBitmap->size().width()+10)*i, 10)));
		}
	}
	else if ((int)m_bonusCounters.size() > m_nbMiniBulletShots)
	{
		for (int i = m_bonusCounters.size()-1; i >= m_nbMiniBulletShots; --i)
		{
			delete m_bonusCounters[i];
		}
		for (int i = m_bonusCounters.size()-1; i >= m_nbMiniBulletShots; --i)
		{
			m_bonusCounters.pop_back();
		}
	}

}

void Cannon::checkCollisions(WaterDrop* waterDrop)
{
	
}

void Cannon::draw() const
{
	if (m_hasCannonDamage)
	{
		m_cannonDamaged->draw();
	}
	else
	{
		m_anim[m_animFrame]->draw();
	}
	iterateListConst(m_cannonBullets, CannonBullet*)
	{
		(*it)->draw();
	}
	for (int i = 0; i < 10; ++i)
	{
		m_cannonBulletForMachine->draw(Int2(1080,540+(int)m_distBulletMachineUp+i*m_cannonBulletForMachine->size().height()), 1.f, Float2(1.f,1.f));
	}
}

void Cannon::drawBonusesCounter() const
{
	iterateVectorConst(m_bonusCounters, BonusCounter*)
	{
		(*it)->draw();
	}
}

Cannon::~Cannon()
{
	iterateVector(m_anim, Sprite*)
	{
		delete *it;
	}
	iterateList(m_cannonBullets, CannonBullet*)
	{
		delete (*it);
	}
	delete m_cannonBulletForMachine;
	delete m_cannonCollisionImage;
	delete m_cannonDamaged;
	delete m_bonusCounterBitmap;
	Engine::instance().getSoundMgr().removeSound(m_soundFire);
	Engine::instance().getSoundMgr().removeSound(m_soundFireMiniBullets);
}

//---------------------------------------------------------------------
