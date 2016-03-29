#ifndef MessageDialog_h_INCLUDED
#define MessageDialog_h_INCLUDED

#include <string>

#include "CoreUtils.h"

class MessageDialogPrivate;

//---------------------------------------------------------------------------------------------

class MessageDialog
{
public:
	MessageDialog();
	void setMessage(std::string text, int64_t durationInSeconds);
	void draw(bool isSmallWindow) const;
	void forceHideMessage();
	void setColorText(const Color& color);
	void setColorBack(const Color& color);
	void setSizeText(float sizeText);
	~MessageDialog();
private:
	MessageDialogPrivate* messageDialogPrivate;
};

//---------------------------------------------------------------------------------------------

#endif //MessageDialog_h_INCLUDED
