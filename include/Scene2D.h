#ifndef Scene2D_h_INCLUDED
#define Scene2D_h_INCLUDED

#include "global_defines.h"
#include "CoreUtils.h"

class OpenGL;

//---------------------------------------------------------------------
/**
* \brief The manager of the 2D scene (singleton).
*/

class Scene2D
{
	friend class Scene3D;
	friend class Engine;
public:
	void set2DMode();

	//! get window size in pixel units
	Int2 getWindowSize() const;
	Int2 getWindowRealSize() const;
	void setPixelPerPoint(const Float2& ppp, const Int2& topLeftCornerInWindow, const Int2& windowVirtualSize);
	inline float getWindowAspectRatio() const { return (float)this->getWindowSize().width() / (float)this->getWindowSize().height(); }
	Int2 convertVirtualPositionToRealPosition(const Int2& pos) const;
	
	//! draw a rectangle
	void drawRectangle(const CoreUtils::RectangleInt& rect, const Color& color, bool isFilled)
	{
		drawRectangle(rect.topLeft, rect.bottomRight, color, isFilled);
	}
	void drawRectangle(const Int2& posLeftTop, const Int2& posRightBottom, const Color& color, bool isFilled)
	{
		drawRoundedRectangle(posLeftTop, posRightBottom, color, 0.f, 1.f, isFilled);
	}

	//! draw a rectangle with rounds (rounds are not supported with opengl)
	void drawRoundedRectangle(
		const Int2& posLeftTop, const Int2& posRightBottom, const Color& color, float round, float borderSize, bool fill);
	
	//! draw a line
	void drawLine(const Int2& pos1, const Int2& pos2, const Color& color, float thickness = 1.f);

	//! Display the text. Return the position reached at right.
	Int2 drawText(
		const char* fontNameOrNullForDefault, const char* text, const Int2& position, float fontSize, const Color& color, 
		bool isCenterScreenX = false, bool isCenterScreenY = false, bool smooth = false, int automaticLineReturn = -1);
	//! Return the width,height of the string in pixels if it were displayed.
	Int2 getSizeText(const char* fontNameOrNullForDefault, const char* text, float fontSize, int nbCharsToRead = -1) const;

	//! Is the default font able to draw this character
	bool isDrawableCharacter(char c) const;

	//void drawTexture(Texture* texture);

	//! note: may return NULL
	static const char* getDefaultFontName();

	void showSettingsCharm();
	
private:

	Scene2D();
	~Scene2D();

	//! fill the screen (for public usage, use Engine method instead)
	void clearScreen(const Color& color);
};

//---------------------------------------------------------------------

#endif //Scene2D_h_INCLUDED
