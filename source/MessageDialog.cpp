
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	#include <wrl.h>
	#include <d3d11_1.h>
	#include <d2d1_1.h>
	#include <d2d1effects.h>
	#include <dwrite_1.h>
	#include <wincodec.h>
#endif

#include "../include/Utils.h"
#include "../include/Engine.h"
#include "../include/CoreUtils.h"

#include "../include/MessageDialog.h"

//---------------------------------------------------------------------------------------------

namespace
{
	const Int2 RECTANGLE_MESSAGE_ON_SCREEN_HALF_SIZE = Int2(350, 200);
}

//---------------------------------------------------------------------------------------------

class MessageDialogPrivate
{
public:
	MessageDialogPrivate() :m_messageOnScreen(""), m_endTimeMessageOnScreen(-1), m_colorText(CoreUtils::colorWhite), m_colorBack(0, 0, 0, 150) {}
	std::string m_messageOnScreen;
	int64_t m_startTimeMessageOnScreen;
	int64_t m_endTimeMessageOnScreen;
	//Windows::UI::Popups::MessageDialog^ msg;
	Color m_colorText;
	Color m_colorBack;
};

//---------------------------------------------------------------------------------------------

MessageDialog::MessageDialog() : messageDialogPrivate(new MessageDialogPrivate)
{
}

//---------------------------------------------------------------------------------------------

void MessageDialog::setColorText(const Color& color)
{
	messageDialogPrivate->m_colorText = color;
}

//---------------------------------------------------------------------------------------------

void MessageDialog::setColorBack(const Color& color)
{
	messageDialogPrivate->m_colorBack = color;
}

//---------------------------------------------------------------------------------------------

void MessageDialog::setMessage(std::string text, int64_t durationInSeconds)
{
	messageDialogPrivate->m_messageOnScreen = text;
	messageDialogPrivate->m_startTimeMessageOnScreen = Utils::getSecond();
	messageDialogPrivate->m_endTimeMessageOnScreen = messageDialogPrivate->m_startTimeMessageOnScreen + durationInSeconds;
	//messageDialogPrivate->msg = ref new Windows::UI::Popups::MessageDialog(Utils::stringToPlatformString(std::string(text)));
	//messageDialogPrivate->msg->ShowAsync();
}

//---------------------------------------------------------------------------------------------

void MessageDialog::draw(bool isSmallWindow) const
{
	long long int timeNow = Utils::getSecond();
	if (messageDialogPrivate->m_messageOnScreen != "")
	{
		if (timeNow >= messageDialogPrivate->m_startTimeMessageOnScreen && timeNow < messageDialogPrivate->m_endTimeMessageOnScreen)
		{
			Scene2D& scene2D = Engine::instance().getScene2DMgr();
			Int2 centerScreen = scene2D.getWindowSize()*0.5f;
			Int2 rectHalf = RECTANGLE_MESSAGE_ON_SCREEN_HALF_SIZE*(isSmallWindow ? 0.5f : 1.f);
			scene2D.drawRoundedRectangle(centerScreen - rectHalf, centerScreen + rectHalf, messageDialogPrivate->m_colorBack, 10, 0, true);
			scene2D.drawText(NULL, messageDialogPrivate->m_messageOnScreen.c_str(), Int2(10, 10), isSmallWindow ? 15.f : 30.f, messageDialogPrivate->m_colorText, true, true);
			//Engine::instance().getScene2DMgr().drawText(NULL, "[ X ]", centerScreen-rectHalf, 30.f, colorWhite, false);
			//std::stringstream sstr; sstr << sizeof(long) << ";" << Utils::getMillisecond() << "; " << m_endTimeMessageOnScreen;
			//Engine::instance().getScene2DMgr().drawText(NULL, sstr.str().c_str(), Int2(10, 10), Button::isSmallWindow() ? 15.f : 30.f, colorWhite, true, true);
		}
	}
	else if (messageDialogPrivate->m_messageOnScreen != "")
	{
		messageDialogPrivate->m_messageOnScreen = "";
	}
}

//---------------------------------------------------------------------------------------------

void MessageDialog::forceHideMessage()
{
	messageDialogPrivate->m_startTimeMessageOnScreen = -1;
	messageDialogPrivate->m_endTimeMessageOnScreen = -1;
	messageDialogPrivate->m_messageOnScreen = "";
}

//---------------------------------------------------------------------------------------------

MessageDialog::~MessageDialog()
{
	delete messageDialogPrivate;
}

//---------------------------------------------------------------------------------------------
