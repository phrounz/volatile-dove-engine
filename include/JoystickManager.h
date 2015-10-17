#ifndef JoystickManager_h_INCLUDED
#define JoystickManager_h_INCLUDED

#include <set>
#include <utility>

//---------------------------------------------------------------------
/**
* \brief The XBox 360 controller inputs manager (singleton).
*/

class JoystickManager
{
	friend class Engine;
public:
	enum PlayerId { PLAYER_ONE = 0, PLAYER_TWO = 1, PLAYER_THREE = 2, PLAYER_FOUR = 3 };
	enum Button { 
		BUTTON_LEFT, BUTTON_RIGHT, BUTTON_UP, BUTTON_DOWN, BUTTON_X, BUTTON_Y, BUTTON_A, BUTTON_B, BUTTON_START, BUTTON_BACK, 
		BUTTON_LEFT_SHOULDER, BUTTON_RIGHT_SHOULDER, BUTTON_LEFT_TRIGGER, BUTTON_RIGHT_TRIGGER, 

		LEFT_STICK_LEFT_AS_BUTTON, LEFT_STICK_RIGHT_AS_BUTTON, LEFT_STICK_UP_AS_BUTTON, LEFT_STICK_DOWN_AS_BUTTON, 
		RIGHT_STICK_LEFT_AS_BUTTON, RIGHT_STICK_RIGHT_AS_BUTTON, RIGHT_STICK_UP_AS_BUTTON, RIGHT_STICK_DOWN_AS_BUTTON };

	enum Stick { LEFT_STICK_X, LEFT_STICK_Y, RIGHT_STICK_X, RIGHT_STICK_Y };

	bool isConnected(PlayerId playerId) const;
	bool isPressed(PlayerId playerId, Button button) const;

	inline bool isPressedNoRepeat(PlayerId playerId, Button button)
	{
		std::pair<PlayerId,Button> key(playerId, button);
		bool isPressed = this->isPressed(playerId, button);
		if (isPressed && m_buttonsWaitForNoRepeat.find(key) == m_buttonsWaitForNoRepeat.end())
		{
			m_buttonsWaitForNoRepeat.insert(key);
			return true;
		}
		if (!isPressed && m_buttonsWaitForNoRepeat.find(key) != m_buttonsWaitForNoRepeat.end())
		{
			m_buttonsWaitForNoRepeat.erase(key);
		}
		return false;
	}

	/*inline void resetButtonsNotPressed()
	{
		iterateSetConst(m_buttonsWaitForNoRepeat, std::pair<PlayerId,Button>)
		{
			if (!this->isPressed((*it).first, (*it).second))
			{
				m_buttonsWaitForNoRepeat.erase(it);
				break;
			}
		}
	}*/

	float getIntensityStick(PlayerId playerId, Stick stick) const;//between -1.f and 1.f
	void vibrate(PlayerId playerId, float vibrationLeft, float vibrationRight);//between -0.f and 1.f
	static JoystickManager::Button convertStrToButton(const std::string& str);

private:
	JoystickManager() {}
	~JoystickManager() {}
	std::set< std::pair<PlayerId,Button> > m_buttonsWaitForNoRepeat;
};

//---------------------------------------------------------------------

#endif //JoystickManager_h_INCLUDED