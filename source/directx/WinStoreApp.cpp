#if defined(USES_WINDOWS8_METRO) && !defined(USES_WINDOWS8_METRO_WITH_XAML)

#include <collection.h>
#include <ppltasks.h>

#include "DirectXDraw.h"
#include "BasicTimer.h"
#include "../../include/Utils.h"
#include "../../include/KeyboardManager.h"

#include "../../include/AbstractMainClass.h"

#include "../AppSetup.h"
#include "../EngineError.h"

#include "WinStoreApp.h"

#include "WinStoreAppWindow.h"

using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

WinStoreApp::WinStoreApp() :m_windowClosed(false), m_mainClass(NULL)
{
}

void WinStoreApp::OnEdgeGestureCompleted(_In_ Windows::UI::Input::EdgeGesture^ userEdgeGesture, _In_ Windows::UI::Input::EdgeGestureEventArgs^ args)
{
}

void WinStoreApp::CreateDeviceIndependentResources()
{
    DirectXBase::CreateDeviceIndependentResources();
	this->CreateSettingsPanelRessources();
}

void WinStoreApp::OnEdgeSettingsButton(Windows::UI::Popups::IUICommand^ command)
{
	m_mainClass->onEdgeSettingsButton(Utils::platformStringToString(command->Id->ToString()).c_str());
}

void WinStoreApp::CreateSettingsPanelRessources()
{
	using namespace Windows::UI::ApplicationSettings;

	SettingsPane::GetForCurrentView()->CommandsRequested += ref new TypedEventHandler<SettingsPane^,SettingsPaneCommandsRequestedEventArgs^>(
    [=](SettingsPane^ sender, SettingsPaneCommandsRequestedEventArgs^ args)
    {
		using namespace Windows::UI::Popups;
		std::vector<std::pair<std::string, std::string>> charmSettings = m_mainClass->getCharmSettings();
		for (std::vector<std::pair<std::string, std::string>>::const_iterator it = charmSettings.begin(); it != charmSettings.end(); it++)
		{
			args->Request->ApplicationCommands->Append(
				ref new SettingsCommand(
					Utils::stringToPlatformString((*it).first.c_str()),
					Utils::stringToPlatformString((*it).second.c_str()),
					ref new UICommandInvokedHandler(this, &WinStoreApp::OnEdgeSettingsButton)
					)
				);
		}
    });
}

void WinStoreApp::Initialize(CoreApplicationView^ applicationView)
{
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &WinStoreApp::OnActivated);

    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &WinStoreApp::OnSuspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &WinStoreApp::OnResuming);
}

void WinStoreApp::SetWindow(CoreWindow^ window)
{
	m_winStoreAppControls.initControls(window, m_mainClass);
	
	window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &WinStoreApp::OnWindowClosed);

    CoreWindow::GetForCurrentThread()->SizeChanged += 
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &WinStoreApp::OnWindowSizeChanged);

	//auto info = DisplayInformation::GetForCurrentView();
	//info->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &WinStoreApp::OnLogicalDpiChanged); 
	DisplayProperties::LogicalDpiChanged += ref new DisplayPropertiesEventHandler(this, &WinStoreApp::OnLogicalDpiChanged);
	
	AbstractWindow* abstractWindow = new WinStoreAppWindow(CoreWindow::GetForCurrentThread());
	DirectXBase::Initialize(abstractWindow, NULL, -1, -1);
}

void WinStoreApp::OnWindowClosed(
	_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

void WinStoreApp::Load(Platform::String^ entryPoint)
{
}

void WinStoreApp::Run()
{
	// register handler for edge gesture completion events
	Windows::UI::Input::EdgeGesture::GetForCurrentView()->Completed +=
		ref new TypedEventHandler<Windows::UI::Input::EdgeGesture^,
		Windows::UI::Input::EdgeGestureEventArgs^>(this, &WinStoreApp::OnEdgeGestureCompleted);

	Assert(!Engine::instance().isInit());
	Engine::instance().initLowLevel();

	try
	{
		m_mainClass->init();
		DirectXBase::startComputingFrameDuration();

		while (!m_windowClosed)
		{	
			//------------ update

			bool needProcessAgain = m_mainClass->update();

			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(
				needProcessAgain ? CoreProcessEventsOption::ProcessAllIfPresent : CoreProcessEventsOption::ProcessOneAndAllPending);

			Engine::instance().getSoundMgr().manage();

			//------------ render

			this->BeginDraw();
			m_mainClass->render();
			this->EndDraw();
			Engine::instance().m_frameDuration = DirectXBase::computeFrameDuration();
		}
	}
	catch (EngineError e)
	{
		outputln("Caught EngineError");
		while (!m_windowClosed)
		{
			if (isDrawing()) EndDraw();
			m_winStoreAppControls.setCrashedState();

			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			
			this->BeginDraw();
			Engine::instance().clearScreen(CoreUtils::colorBlack);
			Engine::instance().getScene2DMgr().drawText(NULL, e.getFullText().c_str(), Int2(20, 40), 18, CoreUtils::colorWhite);
			this->EndDraw();
			Engine::instance().m_frameDuration = DirectXBase::computeFrameDuration();
		}
	}
	
	m_mainClass->deinit();
	Engine::instance().deinitLowLevel();
}

void WinStoreApp::Uninitialize()
{
	uninitializeWindow();
}

void WinStoreApp::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	AppSetup::instance().onResizeWindow(Int2((int)(args->Size.Width), (int)(args->Size.Height)));
	UpdateForWindowSizeChange();
	Engine::instance().onWindowResizeInternals();
	m_mainClass->onWindowResize();
}

void WinStoreApp::OnLogicalDpiChanged(Platform::Object^ sender)
{
	SetDpi(static_cast<float>(DisplayProperties::LogicalDpi));
}

void WinStoreApp::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    CoreWindow::GetForCurrentThread()->Activate();
}

void WinStoreApp::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    // Save application state after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations.
    // Be aware that a deferral may not be held indefinitely. After about five
    // seconds, the application will be forced to exit.
    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	DirectXBase::OnSuspending(sender, args);
	
    // Insert your code here
	this->m_mainClass->onSuspending();
	Engine::instance().getSoundMgr().onSuspending();

    deferral->Complete();
}
 
void WinStoreApp::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	Engine::instance().getSoundMgr().onResuming();
}

#endif
