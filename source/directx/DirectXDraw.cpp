
#include "DirectXHelper.h"

#include "DirectXDraw.h"

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

namespace DirectXDraw
{
	void drawBitmap(
		ID2D1DeviceContext* context, const Float2& zoomRatio, const Float2& m_defaultDpi, 
		ID2D1Bitmap* bitmap, const Float2& position, const Float2& scale, float opacity, D2D1_RECT_F* rectSource, D2D1_RECT_F* rectDest, float angleDegree, const Int2& rotationCenterPos, 
		bool horizontalMirror, bool verticalMirror)
	{
		D2D1_MATRIX_3X2_F transformTmp;
		context->GetTransform(&transformTmp);

		D2D1::Matrix3x2F matTranslation = D2D1::Matrix3x2F::Translation(zoomRatio.x() + position.x(), zoomRatio.y() + position.y());//  / scale.x() / scale.y()
		
		if (angleDegree != 0.f || horizontalMirror || verticalMirror)
		{
			D2D1::Matrix3x2F matMirroring = D2D1::Matrix3x2F::Scale(horizontalMirror ? -1.f : 1.f, verticalMirror ? -1.f : 1.f);
			D2D1_POINT_2F imageRotationCenterPos = D2D1::Point2F((float)rotationCenterPos.x(), (float)rotationCenterPos.y());
			D2D1::Matrix3x2F matRotation = D2D1::Matrix3x2F::Rotation(angleDegree, imageRotationCenterPos);
			context->SetTransform(matMirroring * matRotation * matTranslation);
		}
		else
		{
			context->SetTransform(matTranslation);// * s_ppp.x() * s_ppp.y()
		}

		//Assert(scale.x() > 0.f && scale.y() > 0.f);
		
		// note about bitmaps: https://msdn.microsoft.com/en-us/library/windows/desktop/dd371150%28v=vs.85%29.aspx
		Float2 dpiBitmap;
		bitmap->GetDpi(&dpiBitmap.data[0], &dpiBitmap.data[1]);

		Float2 scaleToApply = dpiBitmap / m_defaultDpi * scale;//s_ppp * dpiBitmap / dpiContext * scale;

		D2D1_RECT_F rectDestFinal;
		if (rectDest == 0)
		{
			D2D1_SIZE_F sz = bitmap->GetSize();
			rectDestFinal.left = 0.f;
			rectDestFinal.top = 0.f;
			rectDestFinal.right = sz.width * scaleToApply.x();
			rectDestFinal.bottom = sz.height *scaleToApply.y();

			context->DrawBitmap(bitmap, &rectDestFinal, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, rectSource);
		}
		else
		{
			rectDestFinal = *rectDest;
			rectDestFinal.left *= scaleToApply.x();
			rectDestFinal.top *= scaleToApply.y();
			rectDestFinal.right *= scaleToApply.x();
			rectDestFinal.bottom *= scaleToApply.y();

			Float2 srcScaleToApply = dpiBitmap / m_defaultDpi;

			D2D1_RECT_F rectSourceFinal;
			rectSourceFinal = *rectSource;
			rectSourceFinal.left /= srcScaleToApply.x();
			rectSourceFinal.top /= srcScaleToApply.y();
			rectSourceFinal.right /= srcScaleToApply.x();
			rectSourceFinal.bottom /= srcScaleToApply.y();

			context->DrawBitmap(bitmap, &rectDestFinal, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &rectSourceFinal);
		}

		context->SetTransform(transformTmp);//D2D1::Matrix3x2F::Translation(0.f, 0.f));
		//s_context->Flush();
	}

	void drawRoundedRectangle(ID2D1DeviceContext* context, const Float2& zoomRatio, const Int2& posLeftTop, const Int2& posRightBottom, const Color& color, float round, float borderSize, bool fill)
	{
		D2D1::ColorF colorDx(color.r() / 256.f, color.g() / 256.f, color.b() / 256.f, color.a() / 256.f);
		ID2D1SolidColorBrush* solidBrush;
		DX::ThrowIfFailed(context->CreateSolidColorBrush(colorDx, &solidBrush));

		D2D1_ROUNDED_RECT rrect;
		rrect.radiusX = round;
		rrect.radiusY = round;
		rrect.rect.left = zoomRatio.x() + (float)posLeftTop.x();
		rrect.rect.top = zoomRatio.y() + (float)posLeftTop.y();
		rrect.rect.right = zoomRatio.x() + (float)posRightBottom.x();
		rrect.rect.bottom = zoomRatio.y() + (float)posRightBottom.y();
		context->SetTransform(D2D1::Matrix3x2F::Identity());

		if (fill) context->FillRoundedRectangle(rrect, solidBrush);
		//if (borderSize > 0.f) 
		context->DrawRoundedRectangle(rrect, solidBrush, borderSize);

		solidBrush->Release();
		//delete solidBrush;
	}

	void drawLine(ID2D1DeviceContext* context, const Float2& zoomRatio, const Int2& pos1, const Int2& pos2, const Color& color, float thickness)
	{
		D2D1::ColorF colorDx(color.r() / 256.f, color.g() / 256.f, color.b() / 256.f, color.a() / 256.f);
		//D2D1::ColorF colorDx = D2D1::ColorF::MidnightBlue;
		ID2D1SolidColorBrush* solidBrush;
		DX::ThrowIfFailed(context->CreateSolidColorBrush(colorDx, &solidBrush));

		D2D1_POINT_2F pt1;
		pt1.x = zoomRatio.x() + (float)pos1.x();
		pt1.y = zoomRatio.y() + (float)pos1.y();
		D2D1_POINT_2F pt2;
		pt2.x = zoomRatio.x() + (float)pos2.x();
		pt2.y = zoomRatio.y() + (float)pos2.y();
		context->SetTransform(D2D1::Matrix3x2F::Identity());
		//s_context->SetDpi(DirectXDraw::getDefaultDpi().x(), DirectXDraw::getDefaultDpi().y());

		context->DrawLine(pt1, pt2, solidBrush, thickness);

		solidBrush->Release();
		//delete solidBrush;
	}

	ID2D1Bitmap* createBitmap(ID2D1DeviceContext* context, const D2D1_SIZE_U& sizeBitmap)
	{
		ID2D1Bitmap* bitmap;
		D2D1_PIXEL_FORMAT pixelFormatBitmap;
		pixelFormatBitmap.format = DXGI_FORMAT_R8G8B8A8_UNORM;// _SRGB;
		// http://msdn.microsoft.com/en-us/library/windows/desktop/dd368058%28v=vs.85%29.aspx
		// http://msdn.microsoft.com/en-us/library/windows/desktop/dd756766%28v=vs.85%29.aspx
		pixelFormatBitmap.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		D2D1_BITMAP_PROPERTIES propertiesBitmap;
		propertiesBitmap.pixelFormat = pixelFormatBitmap;
		// http://fr.wikipedia.org/wiki/Point_par_pouce
		propertiesBitmap.dpiX = 0.f;//100.f / scale;
		propertiesBitmap.dpiY = 0.f;//100.f / scale;
		HRESULT hr = context->CreateBitmap(sizeBitmap, propertiesBitmap, &bitmap);
		Utils::checkHResult(hr);

		return bitmap;
	}
#ifdef ENABLE_DIRECT_WRITE
	IDWriteTextFormat* createTextFormat(IDWriteFactory1* writeFactory, const char* fontName, bool alignmentCenterX, bool alignmentCenterY)
	{
		IDWriteTextFormat* textFormat;

		// Create a DirectWrite text format object.
		DX::ThrowIfFailed(
			writeFactory->CreateTextFormat(
			Utils::convertStringToWString(fontName).c_str(),//Gabriola
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			64.f,
			L"en-US", // Locale
			&textFormat
			)
			);

		// Center the text horizontally and vertically.
		textFormat->SetTextAlignment(alignmentCenterX ? DWRITE_TEXT_ALIGNMENT_CENTER : DWRITE_TEXT_ALIGNMENT_LEADING);
		textFormat->SetParagraphAlignment(alignmentCenterY ? DWRITE_PARAGRAPH_ALIGNMENT_CENTER : DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

		return textFormat;
	}

	ID2D1SolidColorBrush* createSolidColorBrush(ID2D1DeviceContext* context, const Color& color)
	{
		D2D1::ColorF colorDx(color.r() / 256.f, color.g() / 256.f, color.b() / 256.f, color.a() / 256.f);
		//D2D1::ColorF colorDx = D2D1::ColorF::MidnightBlue;

		ID2D1SolidColorBrush* solidBrush;
		DX::ThrowIfFailed(context->CreateSolidColorBrush(colorDx, &solidBrush));
		return solidBrush;
	}

	IDWriteTextLayout* createTextLayout(IDWriteFactory1* writeFactory, IDWriteTextFormat* textFormat, const Int2& layoutSize, float scale, const wchar_t* wtext)
	{
		IDWriteTextLayout* textLayout;

		// Create a DirectWrite Text Layout object.
		DX::ThrowIfFailed(
			writeFactory->CreateTextLayout(
			wtext,              // Text to be displayed
			(UINT32)wcslen(wtext),      // Length of the text
			textFormat,				// DirectWrite Text Format object
			layoutSize.width() / scale,         // Width of the Text Layout
			layoutSize.height() / scale,        // Height of the Text Layout
			&textLayout
			)
			);

		// Create a text range corresponding to the entire string.
		DWRITE_TEXT_RANGE textRange = { 0 };
		textRange.length = static_cast<UINT32>(wcslen(wtext));
		textRange.startPosition = 0;

		// Set the font size and weight on the text range.
		textLayout->SetFontSize(100.f, textRange);
		textLayout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, textRange);
		//m_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		return textLayout;
	}

	void drawTextLayout(ID2D1DeviceContext* context, const Float2& zoomRatio, ID2D1SolidColorBrush* solidBrush, IDWriteTextLayout* textLayout, const Int2& position, const Float2& scale)
	{
		D2D1_MATRIX_3X2_F transformTmp;
		context->GetTransform(&transformTmp);
		context->SetTransform(D2D1::Matrix3x2F::Translation(zoomRatio.x() + (float)position.x() / scale.x(), zoomRatio.y() + (float)position.y() / scale.y()));// * s_ppp.x() * s_ppp.y(
		// TODO scale with s_ppp

		context->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), textLayout, solidBrush);

		context->SetTransform(transformTmp);
	}
#endif
}


//---------------------------------------------------------------------
/*
void Scene2D::drawTexture(Texture* texture)
{
s_context->SetTransform(D2D1::Matrix3x2F::Identity());
s_context->SetDpi(DirectXDraw::defaultDpiX, DirectXDraw::defaultDpiY);
//s_context->Draw();
texture->setAsCurrentTexture();
BasicVertex v[3];
v[0].pos = Float3(0.f, 0.f, 0.f);
v[1].pos = Float3(0.f, 1.f, 0.f);
v[2].pos = Float3(1.f, 0.f, 0.f);
UINT stride = sizeof(BasicVertex);
UINT offset = 0;
DXMain::getContext()->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
DXMain::getContext()->Draw(1, &v);
}
*/


#endif
