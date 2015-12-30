#pragma once
#if defined(USES_WINDOWS8_METRO) && !defined(USES_WINDOWS8_METRO_WITH_XAML)

#include "directx_inc.h"

#include <string>

#include "../../include/Engine.h"
#include "../../include/AbstractMainClass.h"
#include "WinStoreAppControls.h"
#include "DirectXBase.h"

ref class WinStoreApp sealed : public DirectXBase, public Windows::ApplicationModel::Core::IFrameworkView
{
public:
    WinStoreApp();

    // DirectXBase Methods
    virtual void CreateDeviceIndependentResources() override;
	
    // IFrameworkView Methods
    virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
    virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
    virtual void Load(Platform::String^ entryPoint);
    virtual void Run();
    virtual void Uninitialize();
	
	//virtual void RunStepBegin() override;
	//virtual void RunStepEnd() override;
	
internal:

	void init(AbstractMainClass* abstractMainClass) { m_mainClass = abstractMainClass; }
	
protected:
    // Event Handlers
    void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
    void OnLogicalDpiChanged(Platform::Object^ sender);
    void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
    void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
    void OnResuming(Platform::Object^ sender, Platform::Object^ args);
	void OnEdgeGestureCompleted(
		_In_ Windows::UI::Input::EdgeGesture^ userEdgeGesture,
		_In_ Windows::UI::Input::EdgeGestureEventArgs^ args);
	
	void OnWindowClosed(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::CoreWindowEventArgs^ args);

private:
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_solidBrush;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
    Microsoft::WRL::ComPtr<IDWriteTextLayout> m_textLayout;
	AbstractMainClass* m_mainClass;
	bool m_windowClosed;
	std::string m_argv0;
	void CreateSettingsPanelRessources();
	void OnEdgeSettingsButton(Windows::UI::Popups::IUICommand^ command);

	WinStoreAppControls m_winStoreAppControls;
};

#endif