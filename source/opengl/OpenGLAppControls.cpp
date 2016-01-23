
#include "opengl_inc.h"

//---------

#include "../../include/CoreUtils.h"
#include "../../include/Engine.h"
#include "../AppSetup.h"

#include "OpenGLAppControls.h"

//---------------------------------------------------------------------
// static values

namespace
{
	AbstractMainClass* s_mainClass = NULL;
	bool s_eventHappened = false;
	bool s_hasPressedAltF4 = false;
}

//---------------------------------------------------------------------
//function called when a mouse button is pressed

void OpenGLApp_onPointerPressedOrReleased(int button, int state,int x, int y)
{
	Int2 pos = AppSetup::instance().convertRealPositionToVirtualPosition(Int2(x,y));
	if (button==MouseManager::MouseEvent::MOUSE_EVENT_WHEEL_UP)
	{
		s_mainClass->onPointerWheelChanged(120, pos.x(), pos.y());
	}
	else if (button==MouseManager::MouseEvent::MOUSE_EVENT_WHEEL_DOWN)
	{
		s_mainClass->onPointerWheelChanged(-120, pos.x(), pos.y());
	}
	else if (state==MouseManager::MouseEvent::MOUSE_EVENT_PRESS)
	{
		Engine::instance().onPointerPressedInternals(button, x, y);
		s_mainClass->onPointerPressed(button, pos.x(), pos.y());
	}
	else if (state==MouseManager::MouseEvent::MOUSE_EVENT_RELEASE)
	{
		Engine::instance().onPointerReleasedInternals(button, x, y);
		s_mainClass->onPointerReleased(button, pos.x(), pos.y());
	}

	s_eventHappened = true;
}

#ifndef USES_SDL_INSTEAD_OF_GLUT

void OpenGLApp_onPointerPressedOrReleasedTmp(int button, int state,int x, int y)
{
	switch(state)
	{
	case GLUT_WHEEL_UP:OpenGLApp_onPointerPressedOrReleased(button, MouseManager::MouseEvent::MOUSE_EVENT_WHEEL_UP,x,y);break;
	case GLUT_WHEEL_DOWN:OpenGLApp_onPointerPressedOrReleased(button, MouseManager::MouseEvent::MOUSE_EVENT_WHEEL_DOWN,x,y);break;
	case GLUT_DOWN:OpenGLApp_onPointerPressedOrReleased(button, MouseManager::MouseEvent::MOUSE_EVENT_PRESS,x,y);break;
	case GLUT_UP:OpenGLApp_onPointerPressedOrReleased(button, MouseManager::MouseEvent::MOUSE_EVENT_RELEASE,x,y);break;
	default:Assert(false);
	}
}

#endif

//---------------------------------------------------------------------
//functions called when the mouse is moved

void OpenGLApp_onPointerMovingActive(int x,int y)
{
	Int2 pos = AppSetup::instance().convertRealPositionToVirtualPosition(Int2(x,y));
	Engine::instance().onPointerMovedInternals(true, pos.x(), pos.y());
	s_mainClass->onPointerMoved(true, pos.x(), pos.y());
}

void OpenGLApp_onPointerMovingPassive(int x,int y)
{
	Int2 pos = AppSetup::instance().convertRealPositionToVirtualPosition(Int2(x,y));
	Engine::instance().onPointerMovedInternals(false, pos.x(), pos.y());
	s_mainClass->onPointerMoved(false, pos.x(), pos.y());
}

//---------------------------------------------------------------------
//function called when a key is pressed with state down

void OpenGLApp_onKeyDown(unsigned char key,int x, int y)
{
	Engine::instance().onKeyPressedInternals((int)key);
	s_mainClass->onKeyPressed((int)key);
	s_eventHappened = true;
}

void OpenGLApp_onKeyDown(int key,int x, int y)
{
	Engine::instance().onKeyPressedInternals((int)key);
	s_mainClass->onKeyPressed((int)key);
	s_eventHappened = true;
	if (key == KeyboardManager::KEY_F4 && Engine::instance().getKeyboardMgr().getKey(KeyboardManager::KEY_ALT))
	{
		s_hasPressedAltF4 = true;
	}
}

//---------------------------------------------------------------------
//function called when a key is pressed with state up

void OpenGLApp_onKeyUp(unsigned char key,int x, int y)
{
	Engine::instance().onKeyReleasedInternals((int)key);
	s_mainClass->onKeyReleased((int)key);
	s_eventHappened = true;
}

void OpenGLApp_onKeyUp(int key,int x, int y)
{
	Engine::instance().onKeyReleasedInternals((int)key);
	s_mainClass->onKeyReleased((int)key);
	s_eventHappened = true;
}

//---------------------------------------------------------------------
//function called when a special key is pressed with state down

void OpenGLApp_onKeyDownSpecial(int key, int x, int y)
{
	Key newkey = KeyboardManager::convertFromGlut(key);
	if (newkey != -1) OpenGLApp_onKeyDown(newkey,x,y);
}

//---------------------------------------------------------------------
//function called when a key special is pressed with state up

void OpenGLApp_onKeyUpSpecial(int key, int x, int y)
{
	Key newkey = KeyboardManager::convertFromGlut(key);
	if (newkey != -1) OpenGLApp_onKeyUp(newkey,x,y);
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

void OpenGLAppControls::initControls(AbstractMainClass* mainClass)
{	
	this->resetEventHappenedToken(false);
	s_mainClass = mainClass;
#ifndef USES_SDL_INSTEAD_OF_GLUT
	glutMouseFunc(OpenGLApp_onPointerPressedOrReleasedTmp);
	glutMotionFunc(OpenGLApp_onPointerMovingActive);
    glutPassiveMotionFunc(OpenGLApp_onPointerMovingPassive);

	glutKeyboardFunc(OpenGLApp_onKeyDown);
	glutKeyboardUpFunc(OpenGLApp_onKeyUp);
    glutSpecialFunc(OpenGLApp_onKeyDownSpecial);
    glutSpecialUpFunc(OpenGLApp_onKeyUpSpecial);
#endif
}

#ifdef USES_SDL_INSTEAD_OF_GLUT
void OpenGLAppControls::manageSDLEvents()
{
	SDL_Event sdlEvent;
	while( SDL_PollEvent(&sdlEvent) )
	{    
		char ALPHA[] = "abcdefghijklmnopqrstuvwxyz";
		SDL_Keycode sym;
		int c;
		int xMouse, yMouse;
		SDL_GetMouseState(&xMouse, &yMouse);
		MouseManager::ButtonType buttonType;
		switch( sdlEvent.type )
		{			
			// SDL_QUIT event (window close)
			case SDL_QUIT:
				exit(0);
				break;

			// Keyboard event
			// Pass the event data onto PrintKeyInfo()
			case SDL_KEYDOWN:
				sym = sdlEvent.key.keysym.sym;
				if(sym >= SDLK_a && sym <= SDLK_z) c = (int)ALPHA[sym - SDLK_a];
				OpenGLApp_onKeyDown(c, xMouse, yMouse);
				break;

			case SDL_KEYUP:
				sym = sdlEvent.key.keysym.sym;
				if(sym >= SDLK_a && sym <= SDLK_z) c = (int)ALPHA[sym - SDLK_a];
				OpenGLApp_onKeyUp(c, xMouse, yMouse);
				break;

			case SDL_MOUSEBUTTONDOWN:case SDL_MOUSEBUTTONUP:
				switch(sdlEvent.button.button)
				{
				case SDL_BUTTON_LEFT: buttonType = MouseManager::MOUSE_LEFT_BUTTON;
				case SDL_BUTTON_MIDDLE: buttonType = MouseManager::MOUSE_MIDDLE_BUTTON;
				case SDL_BUTTON_RIGHT: buttonType = MouseManager::MOUSE_RIGHT_BUTTON;
				}
				OpenGLApp_onPointerPressedOrReleased(
					(int)buttonType, 
					sdlEvent.button.state == SDL_PRESSED ? MouseManager::MouseEvent::MOUSE_EVENT_PRESS : MouseManager::MouseEvent::MOUSE_EVENT_RELEASE, 
					sdlEvent.button.x, sdlEvent.button.y);
				break;

			default:
				break;
		}

	}
}
#endif

//---------------------------------------------------------------------

bool OpenGLAppControls::hasPressedAltF4()
{
	return s_hasPressedAltF4;
}

//---------------------------------------------------------------------

void OpenGLAppControls::resetEventHappenedToken(bool newValue)
{
	s_eventHappened = newValue;
}

//---------------------------------------------------------------------

bool OpenGLAppControls::hasEventHappenedSinceReset() const
{
	return s_eventHappened;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
