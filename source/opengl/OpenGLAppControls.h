#ifndef OpenGLAppControls_h_INCLUDED
#define OpenGLAppControls_h_INCLUDED

#include "../../include/AbstractMainClass.h"

class OpenGLAppControls
{
public:
	OpenGLAppControls() {}
	void initControls(AbstractMainClass* mainClass);
#ifdef USES_SDL_INSTEAD_OF_GLUT
	void manageSDLEvents();
#endif
	bool hasPressedAltF4();

	void resetEventHappenedToken(bool newValue);
	bool hasEventHappenedSinceReset() const;
};

#endif //OpenGLAppControls_h_INCLUDED
