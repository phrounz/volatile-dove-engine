#ifndef MouseManager_h_INCLUDED
#define MouseManager_h_INCLUDED

#include "CoreUtils.h"

//---------------------------------------------------------------------
/**
* \brief The mouse inputs manager (singleton).
*/

class MouseManager
{
	friend class Engine;
public:

	//---------------------------------------------------------------------

	class MouseEvent
	{
	public:
		enum MouseEventType { MOUSE_EVENT_PRESS, MOUSE_EVENT_RELEASE, MOUSE_EVENT_MOTION, MOUSE_EVENT_PASSIVE_MOTION };
		MouseEvent(const Int2& parMousePosition, int parMouseButton, MouseEventType parEventType) :
			mousePosition(parMousePosition),
			mouseButton(parMouseButton),
			eventType(parEventType)
		{}
		Int2 mousePosition;
		int mouseButton;
		MouseEventType eventType;
	};

	//---------------------------------------------------------------------

	typedef int ButtonType;

	static const ButtonType MOUSE_LEFT_BUTTON;
	static const ButtonType MOUSE_MIDDLE_BUTTON;
	static const ButtonType MOUSE_RIGHT_BUTTON;
	//static const int MOUSE_WHEEL_UP;
	//static const int MOUSE_WHEEL_DOWN;

	bool getMouseButton(int n);

	const Int2& getMousePosition() const {return mousePosition;}

	void initMouseAsNotPressed();

	static std::string convertFromButtonToStr(ButtonType button);

private:

	MouseManager();
	~MouseManager() {}

	void onPointerPressed(ButtonType button, int x, int y);
	void onPointerReleased(ButtonType button, int x, int y);
	void onPointerMoving(bool isClicking, int x, int y);
	void onPointerWheelChanged(int wheelDelta, int x, int y);
	
	bool mouseButton[5];
	bool mouseButtonBefore[5];
	Int2 mousePosition;
	Int2 mouseRealPosition;
	Int2 mousePosBuffer;

	long long int wheel;

	int getButtonTableIdFromMouseId(int n);
};

//---------------------------------------------------------------------

#endif //MouseManager_h_INCLUDED
