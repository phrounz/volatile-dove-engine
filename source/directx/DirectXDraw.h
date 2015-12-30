#pragma once

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

#include "directx_inc.h"

#include "../../include/CoreUtils.h"

namespace DirectXDraw
{
	void drawBitmap(
		ID2D1DeviceContext* context, const Float2& zoomRatio, const Float2& m_defaultDpi,
		ID2D1Bitmap* bitmap, const Float2& position, const Float2& scale, float opacity, D2D1_RECT_F* rectSource, D2D1_RECT_F* rectDest, float angleDegree, const Int2& rotationCenterPos,
		bool horizontalMirror, bool verticalMirror);

	void drawRoundedRectangle(ID2D1DeviceContext* context, const Float2& zoomRatio, const Int2& posLeftTop, const Int2& posRightBottom, const Color& color, float round, float borderSize, bool fill);
	void drawLine(ID2D1DeviceContext* context, const Float2& zoomRatio, const Int2& pos1, const Int2& pos2, const Color& color, float thickness);
	void drawTextLayout(ID2D1DeviceContext* context, const Float2& zoomRatio, ID2D1SolidColorBrush* solidBrush, IDWriteTextLayout* textLayout, const Int2& position, const Float2& scale);

	ID2D1Bitmap* createBitmap(ID2D1DeviceContext* context, const D2D1_SIZE_U& sizeBitmap);

#ifdef ENABLE_DIRECT_WRITE
	IDWriteTextFormat* createTextFormat(IDWriteFactory1* writeFactory, const char* fontName, bool alignmentCenterX, bool alignmentCenterY);
	ID2D1SolidColorBrush* createSolidColorBrush(ID2D1DeviceContext* context, const Color& color);
	IDWriteTextLayout* createTextLayout(IDWriteFactory1* writeFactory, IDWriteTextFormat* textFormat, const Int2& layoutSize, float scale, const wchar_t* wtext);
#endif
}

#endif
