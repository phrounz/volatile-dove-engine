
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	#include "opengl/OpenGL.h"
	#include "opengl/Shader.h"
#else
	#include "directx/DXMain.h"
#endif

#include "ResolutionChanger.h"

#include "Font.h"

#include "AppSetup.h"

AppSetup* AppSetup::s_instance = NULL;

//-------------------------------------------------------------------------

AppSetup::AppSetup(const AppSetupInfos& appSetupInfos) : m_inf(appSetupInfos), m_isUsingVirtualSize(m_inf.automaticFitToWindowSize), m_is3DModeInsteadOf2DMode(false)
{
	Assert(s_instance == NULL);
	s_instance = this;
}

//-------------------------------------------------------------------------

void AppSetup::init()
{
#ifndef USES_WINDOWS8_METRO // no way to change the resolution if USES_WINDOWS8_METRO
	if (m_inf.changeDesktopResolution)
	{
		bool res = ResolutionChanger::changeResolution(m_inf.windowSize.width(), m_inf.windowSize.height());
		AssertMessage(res, "Could not change resolution");
	}
#endif

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	m_openGL = new OpenGL(m_inf.windowTitle.c_str(), m_inf.windowSize, m_inf.isFullscreen, m_inf.automaticFitToWindowSize);
#else
	this->onResizeWindow(this->getWindowRealSize());
#endif

	Font::setDefaultFont(new Font(m_inf.defaultFontImgPath.c_str(), 1.f, 12));
	//this->set2DMode();
}

//-------------------------------------------------------------------------

void AppSetup::onResizeWindow(const Int2& newSize)
{
	m_inf.windowSize = newSize;

	Float2 newSizeF = Float2((float)newSize.width(), (float)newSize.height());
	Float2 virtualSizeF = Float2((float)m_inf.virtualSize.width(), (float)m_inf.virtualSize.height());

	if (m_inf.automaticFitToWindowSize && (newSize.width() != m_inf.virtualSize.width() || newSize.height() != m_inf.virtualSize.height()))
	{
		Float2 ppp(-1.f,-1.f);
		Int2 virtualPos(-1,-1);

		float ratioW = (float)newSize.width() / (float)m_inf.virtualSize.width();
		float ratioH = (float)newSize.height() / (float)m_inf.virtualSize.height();
		if (m_inf.virtualSizeAllowRatioDeformation)
		{
			ppp = Float2(ratioW,ratioH);
			virtualPos = Int2(0,0);
		}
		else if (newSizeF.width()/newSizeF.height() > virtualSizeF.width() / virtualSizeF.height())
		{
			ppp = Float2(ratioH,ratioH);
			virtualPos = Int2((int)((newSizeF.width()-virtualSizeF.width()*ratioH)/2.f), 0);
		}
		else
		{
			ppp = Float2(ratioW,ratioW);
			virtualPos = Int2(0, (int)((newSizeF.height()-virtualSizeF.height()*ratioW)/2.f));
		}

		this->setPixelPerPointLowLevel(ppp, virtualPos);
	}
	else
	{
		this->setPixelPerPointLowLevel(Float2(1.f,1.f), Int2(0,0));
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
		m_openGL->setRealWindowSize(newSize);
		m_openGL->set2DMode();
#else
		// do nothing here
#endif
	}

	m_isUsingVirtualSize = (this->getPixelPerPoint() != Float2(1.f, 1.f) || this->getVirtualTopLeftCornerInWindow() != Int2(0, 0));
}

//-------------------------------------------------------------------------

void AppSetup::deinit()
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	delete m_openGL;
#endif

	Font::deleteDefaultFont();
}

//-------------------------------------------------------------------------

void AppSetup::setPixelPerPoint(const Float2& ppp, const Int2& topLeftCornerInWindow, const Int2& windowVirtualSize)
{
	m_isUsingVirtualSize = (this->getPixelPerPoint() != Float2(1.f, 1.f) || this->getVirtualTopLeftCornerInWindow() != Int2(0, 0));
	m_inf.virtualSize = windowVirtualSize;
	this->setPixelPerPointLowLevel(ppp, topLeftCornerInWindow);
}

//-------------------------------------------------------------------------

void AppSetup::setPixelPerPointLowLevel(const Float2& ppp, const Int2& topLeftCornerInWindow)
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	m_openGL->setPixelPerPoint(m_inf.windowSize, ppp, topLeftCornerInWindow);
#else
	DXMain::instance()->setPixelPerPoint(ppp, topLeftCornerInWindow);
#endif
}

//-------------------------------------------------------------------------

Int2 AppSetup::getWindowVirtualSize() const
{
	return m_isUsingVirtualSize ? m_inf.virtualSize : m_inf.windowSize;
}

//-------------------------------------------------------------------------

Int2 AppSetup::getWindowCurrentRealSize() const
{
	return m_inf.windowSize;
}

//-------------------------------------------------------------------------

Int2 AppSetup::getWindowRealSize() const
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	return m_openGL->getWindowRealSize();
#else
	Int2 output;
	DXMain::instance()->getWindowSize(&output.data[0], &output.data[1]);
	return output;
#endif
}

//-------------------------------------------------------------------------

void AppSetup::set2DMode()
{
	m_is3DModeInsteadOf2DMode = false;
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	Shader::instance().stopUsing();
	m_openGL->set2DMode();
#else
	DXMain::instance()->set2DMode();
#endif
}

//-------------------------------------------------------------------------

void AppSetup::set3DMode(float fov, float minViewDistance, float maxViewDistance)
{
	m_is3DModeInsteadOf2DMode = true;
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	m_openGL->set3DMode(fov, minViewDistance, maxViewDistance);
#else
	DXMain::instance()->set3DMode();
#endif
}

//-------------------------------------------------------------------------

void AppSetup::manageRender()
{
	if (m_isUsingVirtualSize)
	{
		Int2 virtualPos = this->getVirtualTopLeftCornerInWindow();
		Float2 ppp = this->getPixelPerPoint();
		Int2 sizeOrtho2DWindow = this->getSizeOrtho2DWindow();

		this->setPixelPerPointLowLevel(Float2(1.f,1.f), Int2(0,0));

		if (m_inf.virtualSizeBorderColor.a() != 0)
		{
			if (m_inf.virtualSize.width() < sizeOrtho2DWindow.width())
			{
				Engine::instance().getScene2DMgr().drawRectangle(
					Int2(0,0),
					Int2(virtualPos.width(), m_inf.windowSize.height()),
					m_inf.virtualSizeBorderColor, true);
				Engine::instance().getScene2DMgr().drawRectangle(
					Int2(m_inf.windowSize.width()-virtualPos.width(), 0),
					m_inf.windowSize,
					m_inf.virtualSizeBorderColor, true);
			}
			else if (m_inf.virtualSize.height() < sizeOrtho2DWindow.height())
			{
				Engine::instance().getScene2DMgr().drawRectangle(
					Int2(0, 0),
					Int2(m_inf.windowSize.width(), virtualPos.height()),
					m_inf.virtualSizeBorderColor, true);
				Engine::instance().getScene2DMgr().drawRectangle(
					Int2(0, m_inf.windowSize.height()-virtualPos.height()),
					m_inf.windowSize,
					m_inf.virtualSizeBorderColor, true);
			}
		}

		this->setPixelPerPointLowLevel(ppp, virtualPos);
	}

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	m_openGL->manageOpenGL(m_inf.windowSize);
#else

#endif
}

//-------------------------------------------------------------------------

Int2 AppSetup::convertRealPositionToVirtualPosition(const Int2& pos) const
{
	if (m_isUsingVirtualSize)
	{
		Int2 virtualPos = this->getVirtualTopLeftCornerInWindow();
		Int2 sizeOrtho2DWindow = this->getSizeOrtho2DWindow();

		if (m_inf.virtualSize.width() < sizeOrtho2DWindow.width())
		{
			float scaleRatio = (float)m_inf.virtualSize.y() / (float)m_inf.windowSize.y();
			return (pos - virtualPos) * scaleRatio;
		}
		else if (m_inf.virtualSize.height() < sizeOrtho2DWindow.height())
		{
			float scaleRatio = (float)m_inf.virtualSize.x() / (float)m_inf.windowSize.x();
			return (pos - virtualPos) * scaleRatio;
		}
		else
		{
			return pos * m_inf.virtualSize / m_inf.windowSize;
		}
	}
	else
	{
		return pos;
	}
}

//-------------------------------------------------------------------------

Int2 AppSetup::convertVirtualPositionToRealPosition(const Int2& pos) const
{
	if (m_isUsingVirtualSize)
	{
		Int2 virtualPos = this->getVirtualTopLeftCornerInWindow();
		Int2 sizeOrtho2DWindow = this->getSizeOrtho2DWindow();

		if (m_inf.virtualSize.width() < sizeOrtho2DWindow.width())
		{
			float scaleRatio = (float)m_inf.virtualSize.y() / (float)m_inf.windowSize.y();
			return (pos / scaleRatio) + virtualPos;
		}
		else if (m_inf.virtualSize.height() < sizeOrtho2DWindow.height())
		{
			float scaleRatio = (float)m_inf.virtualSize.x() / (float)m_inf.windowSize.x();
			return (pos / scaleRatio) + virtualPos;
		}
		else
		{
			return pos * m_inf.windowSize / m_inf.virtualSize;
		}
	}
	else
	{
		return pos;
	}
}

//-------------------------------------------------------------------------

Float2 AppSetup::getPixelPerPoint() const
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
		return m_openGL->getPixelPerPoint();
#else
	return DXMain::instance()->getPPP();
#endif
}

//-------------------------------------------------------------------------

Int2 AppSetup::getSizeOrtho2DWindow() const
{
	Float2 ppp = this->getPixelPerPoint();
	return Int2((int)((float)m_inf.windowSize.x() / ppp.x()), (int)((float)m_inf.windowSize.y() / ppp.y()));
}

//-------------------------------------------------------------------------

Int2 AppSetup::getVirtualTopLeftCornerInWindow() const
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	return m_openGL->getTopLeftCornerInWindow();
#else
	return DXMain::instance()->getTopLeftCornerInWindow();
#endif
}

//-------------------------------------------------------------------------

bool AppSetup::isWindowFullscreen() const
{
	return m_inf.isFullscreen;
}

//-------------------------------------------------------------------------

std::string AppSetup::getWindowTitle() const
{
	return m_inf.windowTitle;
}

//-------------------------------------------------------------------------

bool AppSetup::isMultisamplingEnabled() const
{
#if defined(USES_WINDOWS8_METRO)
	return false;
#else
	return m_inf.enableMSAA;
#endif
}

//-------------------------------------------------------------------------

AppSetup::~AppSetup()
{
	s_instance = NULL;
}

//-------------------------------------------------------------------------
