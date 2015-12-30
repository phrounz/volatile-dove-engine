#ifndef TextElement_h_INCLUDED
#define TextElement_h_INCLUDED

#include "directx_inc.h"

#include "../../include/CoreUtils.h"

class Font;

//---------------------------------------------------------------------

class TextElement
{
	friend class Engine;
public:
	static void init();
	//TextElement(const char* text, float size = 1.f, const Color& color = colorWhite, bool alignmentCenterX = false, bool alignmentCenterY = false);
	TextElement(const char* fontName, const char* text, float size = 1.f, const Color& color = CoreUtils::colorWhite, bool alignmentCenterX = false, bool alignmentCenterY = false);
	void draw();
	inline void setPosition(const Int2& newPos) {m_pos = newPos;}
	inline Int2 getPosition() const {return m_pos;}
	~TextElement();
private:
	std::string textmem;
	Color colormem;
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	ID2D1SolidColorBrush* m_solidBrush;
	IDWriteTextFormat* m_textFormat;
	IDWriteTextLayout* m_textLayout;
#endif
	float m_scale;
	Int2 m_pos;
};

//---------------------------------------------------------------------

#endif //TextElement_h_INCLUDED
