#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

	#include "directx/DXMain.h"
	#include "directx/TextElement.h"
#else
	#include "opengl/OpenGLDraw.h"
	#include "opengl/SDLDraw.h"
#endif
#include "Font.h"

#include "AppSetup.h"

#include "../include/Scene2D.h"

//---------------------------------------------------------------------

namespace
{
	const char* FONT_NAME_DEFAULT = "Segoe UI Symbol";// 1.5f
}

#define USE_FONT_INSTEAD_OF_TEXTELEMENT

//---------------------------------------------------------------------

Scene2D::Scene2D()
{
}

//---------------------------------------------------------------------

void Scene2D::setVirtualSize(bool yesNo, const Int2& virtualSize, const Color& borderColor)
{
	AppSetup::instance().setVirtualSize(yesNo, virtualSize, borderColor);
}

//---------------------------------------------------------------------
//	Activate the 2D mode to display 2d

void Scene2D::set2DMode()
{
	AppSetup::instance().set2DMode();
}

//---------------------------------------------------------------------

void Scene2D::drawRoundedRectangle(
	const Int2& posLeftTop, const Int2& posRightBottom, const Color& color, float round, float borderSize, bool fill)
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
#ifdef USES_SDL_INSTEAD_OF_GLUT
	SDLDraw::drawRectangle(posLeftTop, posRightBottom, color, fill);
#else
	OpenGLDraw::drawRectangle(posLeftTop, posRightBottom, color, borderSize, fill);
#endif
#else
	DXMain::instance()->drawRoundedRectangle(posLeftTop, posRightBottom, color, round, borderSize, fill);
#endif
}

//---------------------------------------------------------------------

Int2 Scene2D::drawText(
	const char* fontNameOrNullForDefault, const char* text, const Int2& position, float fontSize, const Color& color, bool isCenterScreenX, bool isCenterScreenY, bool smooth, int automaticLineReturn)
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USE_FONT_INSTEAD_OF_TEXTELEMENT) || defined(USES_JS_EMSCRIPTEN)
	Int2 pos = position;
	const Font* font = Font::getDefaultFont();
	int widthText = font->getWidth(text, -1, fontSize / 32.f);
	int heightText = font->getHeight(text, fontSize / 32.f);
	if (isCenterScreenX)
	{
		pos.data[0] = (int)(this->getWindowSize().width()*0.5f - widthText*0.5f);
	}
	if (isCenterScreenY)
	{
		pos.data[1] = (int)(this->getWindowSize().height()*0.5f - heightText*0.5f);
	}

	if (automaticLineReturn != -1)
	{
		return font->drawTextWithLineReturn(color, pos, text, (unsigned int)automaticLineReturn, Float2(fontSize / 32.f, fontSize / 32.f), false, -1, smooth);
	}
	return font->drawText(color, pos, text, Float2(fontSize / 32.f, fontSize / 32.f), -1, false, smooth);
#else
	Assert(automaticLineReturn == -1);//not implemented
	TextElement textMessage(fontNameOrNullForDefault == NULL ? FONT_NAME_DEFAULT : fontNameOrNullForDefault, text, fontSize, color, isCenterScreenX, isCenterScreenY);
	textMessage.setPosition(position);
	textMessage.draw();
	return Int2(0, 0);
#endif
}

//---------------------------------------------------------------------

Int2 Scene2D::getSizeText(const char* fontNameOrNullForDefault, const char* text, float fontSize, int nbCharsToRead) const
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USE_FONT_INSTEAD_OF_TEXTELEMENT) || defined(USES_JS_EMSCRIPTEN)
	const Font* font = Font::getDefaultFont();
	return Int2(font->getWidth(text, nbCharsToRead, fontSize / 32.f), font->getHeight(text, fontSize / 32.f));
#else
	Assert(false);
	return Int2(0,0);
#endif
}

//---------------------------------------------------------------------

bool Scene2D::isDrawableCharacter(char c) const
{
	const Font* font = Font::getDefaultFont();
	return font->isSupported(c);
}

//---------------------------------------------------------------------

void Scene2D::drawLine(const Int2& pos1, const Int2& pos2, const Color& color, float thickness)
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
#ifdef USES_SDL_INSTEAD_OF_GLUT
	SDLDraw::drawLine(pos1, pos2, color);
#else
	OpenGLDraw::drawLine(pos1, pos2, color, thickness);
#endif
#else
	DXMain::instance()->drawLine(pos1, pos2, color, thickness);
#endif
}

//---------------------------------------------------------------------

void Scene2D::clearScreen(const Color& color)
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
#ifdef USES_SDL_INSTEAD_OF_GLUT
	SDLDraw::clearScreen(color);
#else
	OpenGLDraw::clearScreen(color);
#endif
#else
	DXMain::instance()->Clear(color);
#endif
}

//---------------------------------------------------------------------

Int2 Scene2D::getWindowSize() const
{
	return AppSetup::instance().getWindowVirtualSize();
}

//---------------------------------------------------------------------

Int2 Scene2D::getWindowRealSize() const
{
	return AppSetup::instance().getWindowCurrentRealSize();
}

//---------------------------------------------------------------------

/*void Scene2D::setPixelPerPoint(const Float2& ppp, const Int2& topLeftCornerInWindow, const Int2& windowVirtualSize)
{
	AppSetup::instance().setPixelPerPoint(ppp, topLeftCornerInWindow, windowVirtualSize);
}*/

//---------------------------------------------------------------------

Int2 Scene2D::convertVirtualPositionToRealPosition(const Int2& pos) const
{
	return AppSetup::instance().convertVirtualPositionToRealPosition(pos);
}

//---------------------------------------------------------------------

void Scene2D::showSettingsCharm()
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_WINDOWS8_DESKTOP) || defined(USES_JS_EMSCRIPTEN)
	#pragma message("TODO Scene2D::showSettingsCharm")
#else
	Windows::UI::ApplicationSettings::SettingsPane::Show();
#endif
}

//---------------------------------------------------------------------

const char* Scene2D::getDefaultFontName()
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USE_FONT_INSTEAD_OF_TEXTELEMENT) || defined(USES_JS_EMSCRIPTEN)
	return NULL;
#else
	return FONT_NAME_DEFAULT;
#endif
}

//---------------------------------------------------------------------

Scene2D::~Scene2D()
{
}

//---------------------------------------------------------------------
