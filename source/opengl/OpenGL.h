
#include "../../include/CoreUtils.h"

class OpenGL
{
public:
	OpenGL(const char *windowTitle, const Int2& windowSize, bool isFullScreenMode, bool automaticFitToWindowSize);

	void set2DMode();
	void set3DMode(float fov, float minViewDistance, float maxViewDistance);
	void manageOpenGL(const Int2& windowSize);

	void setPixelPerPoint(const Int2& windowRealSize, const Float2& ppp, const Int2& topLeftCornerInWindow);

	Int2 getWindowRealSize() const;
	const Float2& getPixelPerPoint() const { return m_ppp; }
	const Int2& getTopLeftCornerInWindow() const { return m_virtualPos; }

	void setRealWindowSize(const Int2& realWindowSize) { m_realWindowSize = realWindowSize; }

	~OpenGL();

private:
	void init2(const char* windowTitle, const Int2& windowSize);
	void setMode(bool fullscreen, int width, int height);

	Float2 m_ppp;
	bool m_modeFullscreenGlut;
	Int2 m_virtualPos;
	Int2 m_realWindowSize;
};
