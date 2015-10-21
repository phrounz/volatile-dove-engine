
#include "WaterSystem.h"

#include "Chara.h"

namespace
{
	const double SPEED = 0.1875;
	const int64_t TIME_ONE_FRAME = 100;//miliseconds
	const int64_t TIME_ONE_DROP = 100;//miliseconds
	
	const int64_t TIME_FIRST_BONUS = 30000;
	const int64_t TIME_ONE_BONUS = 10000;
}

Chara::Chara()
:m_pos(600.0,510.0), m_animFrame(0), m_timeNextFrame(-1), m_lastDirectionLeft(true), m_timeNextDrop(-1), m_timeNextBonus(Utils::getMillisecond()+TIME_FIRST_BONUS), m_nbCaughtBonuses(0)
{
	m_anim.push_back(new Bitmap("data/chara1.png"));
	m_anim.push_back(new Bitmap("data/chara2.png"));
	m_anim.push_back(new Bitmap("data/chara3.png"));
	m_anim.push_back(new Bitmap("data/chara4.png"));
	m_anim.push_back(new Bitmap("data/chara5.png"));
	m_anim.push_back(new Bitmap("data/chara6.png"));

	m_soundBonusPickup = Engine::instance().getSoundMgr().loadSound("data/sounds/253172__suntemple__retro-bonus-pickup-sfx__mod.wav");
}

void Chara::manage(Image* collisionImage, WaterSystem* waterSystem)
{
	bool keyPressed = false;
	
	KeyboardManager& keyboardMgr = Engine::instance().getKeyboardMgr();
	JoystickManager& joystickMgr = Engine::instance().getJoystickMgr();
	if (keyboardMgr.getKey(KeyboardManager::KEY_LEFT) || joystickMgr.isPressed(JoystickManager::PLAYER_TWO, JoystickManager::BUTTON_LEFT)
		|| joystickMgr.isPressed(JoystickManager::PLAYER_TWO, JoystickManager::LEFT_STICK_LEFT_AS_BUTTON))
	{
		Double2 newPos = m_pos + Double2(-SPEED * Engine::instance().getFrameDuration(), 0.0);
		if (collisionImage->getPixel(CoreUtils::fromDouble2ToInt2(newPos)).a() != 0) m_pos = newPos;
		keyPressed = true;
		m_lastDirectionLeft = true;
	}
	else if (keyboardMgr.getKey(KeyboardManager::KEY_RIGHT) || joystickMgr.isPressed(JoystickManager::PLAYER_TWO, JoystickManager::BUTTON_RIGHT)
		|| joystickMgr.isPressed(JoystickManager::PLAYER_TWO, JoystickManager::LEFT_STICK_RIGHT_AS_BUTTON))
	{
		Double2 newPos = m_pos + Double2(SPEED * Engine::instance().getFrameDuration(), 0.0);
		if (collisionImage->getPixel(CoreUtils::fromDouble2ToInt2(newPos)).a() != 0) m_pos = newPos;
		keyPressed = true;
		m_lastDirectionLeft = false;
	}
	if (keyboardMgr.getKey(KeyboardManager::KEY_UP) || joystickMgr.isPressed(JoystickManager::PLAYER_TWO, JoystickManager::BUTTON_UP)
		|| joystickMgr.isPressed(JoystickManager::PLAYER_TWO, JoystickManager::LEFT_STICK_UP_AS_BUTTON))
	{
		Double2 newPos = m_pos + Double2(0.0, -SPEED * Engine::instance().getFrameDuration());
		if (collisionImage->getPixel(CoreUtils::fromDouble2ToInt2(newPos)).a() != 0) m_pos = newPos;
		keyPressed = true;
	}
	else if (keyboardMgr.getKey(KeyboardManager::KEY_DOWN) || joystickMgr.isPressed(JoystickManager::PLAYER_TWO, JoystickManager::BUTTON_DOWN)
		|| joystickMgr.isPressed(JoystickManager::PLAYER_TWO, JoystickManager::LEFT_STICK_DOWN_AS_BUTTON))
	{
		Double2 newPos = m_pos + Double2(0.0, SPEED * Engine::instance().getFrameDuration());
		if (collisionImage->getPixel(CoreUtils::fromDouble2ToInt2(newPos)).a() != 0) m_pos = newPos;
		keyPressed = true;
	}

	if ((keyboardMgr.getKey(KeyboardManager::KEY_DEL) || joystickMgr.isPressed(JoystickManager::PLAYER_TWO, JoystickManager::BUTTON_X)) && Utils::getMillisecond() > m_timeNextDrop)
	{
		Double2 initSpeed = m_lastDirectionLeft ? Double2(-5,-3) : Double2(5,-3);
		//if (keyboardMgr.getKey(KeyboardManager::KEY_DOWN)) initSpeed = Double2(0.0,1.0);
		waterSystem->addDrop(CoreUtils::fromDouble2ToInt2(m_pos), initSpeed);
		m_timeNextDrop = Utils::getMillisecond() + TIME_ONE_DROP;
	}

	if (keyPressed && Utils::getMillisecond() > m_timeNextFrame)
	{
		m_timeNextFrame = Utils::getMillisecond() + TIME_ONE_FRAME;
		++m_animFrame;
		if (m_animFrame >= 6) m_animFrame = 0;
	}
	
	if (Utils::getMillisecond() > m_timeNextBonus)
	{
		m_timeNextBonus = Utils::getMillisecond() + TIME_ONE_BONUS;

		Int2 pos;
		do
		{
			pos.data[0] = Utils::random(Engine::instance().getScene2DMgr().getWindowSize().width());
			pos.data[1] = Utils::random(Engine::instance().getScene2DMgr().getWindowSize().height());
		}
		while (collisionImage->getPixel(pos).a() == 0);

		if (m_bonuses.size() > 10) m_bonuses.front()->destroy();
		m_bonuses.push_back(new Bonus(pos));
	}

	iterateList(m_bonuses, Bonus*)
	{
		(*it)->manage();
		if ((*it)->isColliding(CoreUtils::fromDouble2ToInt2(m_pos)))
		{
			m_soundBonusPickup->play();
			(*it)->destroy();
			m_nbCaughtBonuses++;
		}
	}
	Autodestroyable::cleanupList(m_bonuses);
}

void Chara::draw() const
{
	if (m_lastDirectionLeft)
	{
		m_anim[m_animFrame]->drawRotated(Int2((int)m_pos.x(), (int)m_pos.y()) -m_anim[m_animFrame]->size()*0.5f, m_anim[m_animFrame]->size(), 0.f, 0.f, true, false, 1.f);
	}
	else
	{
		m_anim[m_animFrame]->draw(Int2((int)m_pos.x(), (int)m_pos.y()) -m_anim[m_animFrame]->size()*0.5f, 1.f, Float2(1.f, 1.f));
	}

	iterateListConst(m_bonuses, Bonus*)
	{
		(*it)->draw();
	}
}

Chara::~Chara()
{
	iterateVector(m_anim, Bitmap*)
	{
		delete *it;
	}

	Autodestroyable::cleanupList(m_bonuses);// TODO crash?
	iterateList(m_bonuses, Bonus*)
	{
		delete *it;
	}

	Engine::instance().getSoundMgr().removeSound(m_soundBonusPickup);
}
