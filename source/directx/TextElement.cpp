
#include "DirectXHelper.h"

#include "DXMain.h"
#include "TextElement.h"
#include "../Font.h"

#include "../../include/Engine.h"
#include "../../include/CoreUtils.h"
#include "../../include/Utils.h"

//---------------------------------------------------------------------

TextElement::TextElement(const char* fontName, const char* text, float size, const Color& color, bool alignmentCenterX, bool alignmentCenterY)
	:m_scale(size), m_pos(0,0)
{
	m_scale /= 100.f;

#ifdef USE_DX_TEXT

	Int2 layoutSize = Engine::instance().getScene2DMgr().getWindowSize();
	std::wstring wtext = Utils::convertStringToWString(text);

	// device independent resources
	m_textFormat = DXMain::instance()->createTextFormat(fontName, alignmentCenterX, alignmentCenterY);

	// device resources
	m_solidBrush = DXMain::instance()->createSolidColorBrush(color);

	// window size dependent resources
	m_textLayout = DXMain::instance()->createTextLayout(m_textFormat, layoutSize, m_scale, wtext.c_str());

#else
	Assert(false);
#endif
}

//---------------------------------------------------------------------
/*
TextElement::TextElement(const char* text, float size, const Color& color, bool alignmentCenterX, bool alignmentCenterY)
	:m_scale(size), m_pos(0,0)
{
	AssertRelease(!alignmentCenterX && !alignmentCenterY);
	m_scale /= 20.f;
	textmem = text;
	colormem = color;
}
*/
//---------------------------------------------------------------------

void TextElement::draw()
{
#ifdef USE_DX_TEXT
	DXMain::instance()->drawTextLayout(m_solidBrush, m_textLayout, m_pos, Float2(m_scale, m_scale));
#endif
}

//---------------------------------------------------------------------

TextElement::~TextElement()
{
	m_textLayout->Release();
	m_textFormat->Release();
	m_solidBrush->Release();
}

//---------------------------------------------------------------------