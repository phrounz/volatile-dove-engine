
#include "../include/Utils.h"
#include "../include/JoystickManager.h"

#if !defined(USES_LINUX) && !defined(USES_JS_EMSCRIPTEN)

// no MFC
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <XInput.h>

namespace
{
	const short int INPUT_DEADZONE = 16000;
}

//---------------------------------------------------------------------

class XBOXController
{
public:
    static XINPUT_STATE getState(int controllerNum)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		XInputGetState(controllerNum, &state);
		return state;
	}
    static bool isConnected(int controllerNum)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		DWORD Result = XInputGetState(controllerNum, &state);
		return (Result == ERROR_SUCCESS ? true : false);
	}
    static void vibrate(int controllerNum, int leftVal = 0, int rightVal = 0)
	{
		XINPUT_VIBRATION Vibration;
		ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));
		Vibration.wLeftMotorSpeed = leftVal;
		Vibration.wRightMotorSpeed = rightVal;
		XInputSetState(controllerNum, &Vibration);
	}
};

//---------------------------------------------------------------------

bool JoystickManager::isConnected(PlayerId playerId) const
{
	return XBOXController::isConnected(playerId);
}

//---------------------------------------------------------------------

bool JoystickManager::isPressed(PlayerId playerId, Button button) const
{
	if (XBOXController::isConnected(playerId))
	{
		switch (button)
		{
		case BUTTON_LEFT:	return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? true : false;
		case BUTTON_RIGHT:	return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? true : false;
		case BUTTON_UP:		return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? true : false;
		case BUTTON_DOWN:	return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? true : false;

		case BUTTON_X:		return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_X ? true : false;
		case BUTTON_Y:		return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_Y ? true : false;
		case BUTTON_A:		return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_A ? true : false;
		case BUTTON_B:		return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_B ? true : false;

		case BUTTON_START:	return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_START ? true : false;
		case BUTTON_BACK:	return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? true : false;

		case BUTTON_LEFT_SHOULDER:	return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? true : false;
		case BUTTON_RIGHT_SHOULDER:	return XBOXController::getState(playerId).Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? true : false;
		case BUTTON_LEFT_TRIGGER:	return XBOXController::getState(playerId).Gamepad.bLeftTrigger > 10;
		case BUTTON_RIGHT_TRIGGER:	return XBOXController::getState(playerId).Gamepad.bRightTrigger > 10;

		case LEFT_STICK_LEFT_AS_BUTTON:		return this->getIntensityStick(playerId, LEFT_STICK_X) < -0.5f;
		case LEFT_STICK_RIGHT_AS_BUTTON:	return this->getIntensityStick(playerId, LEFT_STICK_X) > 0.5f;
		case LEFT_STICK_UP_AS_BUTTON:		return this->getIntensityStick(playerId, LEFT_STICK_Y) > 0.5f;
		case LEFT_STICK_DOWN_AS_BUTTON:		return this->getIntensityStick(playerId, LEFT_STICK_Y) < -0.5f;

		case RIGHT_STICK_LEFT_AS_BUTTON:	return this->getIntensityStick(playerId, RIGHT_STICK_X) < -0.5f;
		case RIGHT_STICK_RIGHT_AS_BUTTON:	return this->getIntensityStick(playerId, RIGHT_STICK_X) > 0.5f;
		case RIGHT_STICK_UP_AS_BUTTON:		return this->getIntensityStick(playerId, RIGHT_STICK_Y) > 0.5f;
		case RIGHT_STICK_DOWN_AS_BUTTON:	return this->getIntensityStick(playerId, RIGHT_STICK_Y) < -0.5f;
		}
	}
	return false;
}

//---------------------------------------------------------------------

float JoystickManager::getIntensityStick(PlayerId playerId, Stick stick) const
{
	if (XBOXController::isConnected(playerId))
	{
		SHORT value = 0;
		switch (stick)
		{
		case LEFT_STICK_X:	value = XBOXController::getState(playerId).Gamepad.sThumbLX;break;
		case LEFT_STICK_Y:	value = XBOXController::getState(playerId).Gamepad.sThumbLY;break;
		case RIGHT_STICK_X:	value = XBOXController::getState(playerId).Gamepad.sThumbRX;break;
		case RIGHT_STICK_Y:	value = XBOXController::getState(playerId).Gamepad.sThumbRY;break;
		default: Assert(false);break;
		}
		return value > INPUT_DEADZONE || value < -INPUT_DEADZONE ? (float)value / (32767.f-(float)INPUT_DEADZONE) : 0.f;
	}
	else
	{
		return 0.f;
	}
}

//---------------------------------------------------------------------

void JoystickManager::vibrate(PlayerId playerId, float vibrationLeft, float vibrationRight)
{
	if (XBOXController::isConnected(playerId))
	{
		XBOXController::vibrate(playerId, (WORD)(vibrationLeft * 65535.f), (WORD)(vibrationRight * 65535.f));
	}
}

//---------------------------------------------------------------------

#else

bool JoystickManager::isConnected(PlayerId playerId) const { return false; }
bool JoystickManager::isPressed(PlayerId playerId, Button button) const { return false; }
float JoystickManager::getIntensityStick(PlayerId playerId, Stick stick) const { return 0.f; }
void JoystickManager::vibrate(PlayerId playerId, float vibrationLeft, float vibrationRight) {}

#endif

//---------------------------------------------------------------------

JoystickManager::Button JoystickManager::convertStrToButton(const std::string& str)
{
	if      (str == "A") return BUTTON_A;
	else if (str == "B") return BUTTON_B;
	else if (str == "X") return BUTTON_X;
	else if (str == "Y") return BUTTON_Y;

	else if (str == "LEFT")				return BUTTON_LEFT;
	else if (str == "RIGHT")			return BUTTON_RIGHT;
	else if (str == "UP")				return BUTTON_UP;
	else if (str == "DOWN")				return BUTTON_DOWN;

	else if (str == "START")			return BUTTON_START;
	else if (str == "BACK")				return BUTTON_BACK;

	else if (str == "LEFT_SHOULDER")	return BUTTON_LEFT_SHOULDER;
	else if (str == "RIGHT_SHOULDER")	return BUTTON_RIGHT_SHOULDER;
	else if (str == "LEFT_TRIGGER")		return BUTTON_LEFT_TRIGGER;
	else if (str == "RIGHT_TRIGGER")	return BUTTON_RIGHT_TRIGGER;

	else if (str == "LEFT_STICK_LEFT_AS_BUTTON")	return LEFT_STICK_LEFT_AS_BUTTON;
	else if (str == "LEFT_STICK_RIGHT_AS_BUTTON")	return LEFT_STICK_RIGHT_AS_BUTTON;
	else if (str == "LEFT_STICK_UP_AS_BUTTON")		return LEFT_STICK_UP_AS_BUTTON;
	else if (str == "LEFT_STICK_DOWN_AS_BUTTON")	return LEFT_STICK_DOWN_AS_BUTTON;

	else if (str == "RIGHT_STICK_LEFT_AS_BUTTON")	return RIGHT_STICK_LEFT_AS_BUTTON;
	else if (str == "RIGHT_STICK_RIGHT_AS_BUTTON")	return RIGHT_STICK_RIGHT_AS_BUTTON;
	else if (str == "RIGHT_STICK_UP_AS_BUTTON")		return RIGHT_STICK_UP_AS_BUTTON;
	else if (str == "RIGHT_STICK_DOWN_AS_BUTTON")	return RIGHT_STICK_DOWN_AS_BUTTON;

	else
	{
		AssertMessage(false, (std::string("Unknown joystick button: ")+str).c_str());
	}
	return (JoystickManager::Button)-1;
}

//---------------------------------------------------------------------
