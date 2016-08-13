#ifndef AppSetup_h_INCLUDED
#define AppSetup_h_INCLUDED

#include "../include/AppSetupInfos.h"

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
class OpenGL;
#endif

class AppSetup
{
public:
	AppSetup(const AppSetupInfos& appSetupInfos);

	const AppSetupInfos& getAppSetupInfos() const { return m_inf; }

	void init();
	void onResizeWindow(const Int2& newSize);
	void setVirtualSize(bool yesNo, const Int2& virtualSize, const Color& borderColor);
	void setPixelPerPoint(const Float2& ppp, const Int2& topLeftCornerInWindow, const Int2& windowVirtualSize);
	Int2 convertRealPositionToVirtualPosition(const Int2& pos) const;
	Int2 convertVirtualPositionToRealPosition(const Int2& pos) const;
	void manageRender();
	void set2DMode();
	void set3DMode(float fov, float minViewDistance, float maxViewDistance);
	void deinit();

	Int2 getWindowVirtualSize() const;
	Int2 getWindowCurrentRealSize() const;
	Int2 getWindowRealSize() const;
	bool isWindowFullscreen() const;
	std::string getWindowTitle() const;
	bool isMultisamplingEnabled() const;
	const Color& getDefaultBackgroundColor() const { return m_inf.defaultBackgroundColor; }
	const Color& getDefaultForegroundColor() const { return m_inf.defaultForegroundColor; }

	bool is3DModeInsteadOf2DMode() const { return m_is3DModeInsteadOf2DMode; }

	static AppSetup& instance() { return *s_instance; }

	~AppSetup();

private:
	AppSetupInfos m_inf;
	bool m_isUsingVirtualSize;
	static AppSetup* s_instance;
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	OpenGL* m_openGL;
#endif

	void setPixelPerPointLowLevel(const Float2& ppp, const Int2& topLeftCornerInWindow);

	Float2 getPixelPerPoint() const;
	Int2 getSizeOrtho2DWindow() const;
	Int2 getVirtualTopLeftCornerInWindow() const;

	bool m_is3DModeInsteadOf2DMode;
};

#endif //AppSetup_h_INCLUDED
