#pragma once
#ifdef USES_WINDOWS8_METRO_WITH_XAML

#include <wrl.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite_1.h>
#include <wincodec.h>
#ifndef VERSION_WINDOWS_8_0
#include <DXGI1_3.h>
#endif
#include <DirectXMath.h>

#include "DirectXBase.h"
#include "WinStoreAppControls.h"

class DXMain;
class AbstractMainClass;
class EngineError;

namespace ScenarioComponent
{
    ref class DirectXBaseFinal sealed : public DirectXBase
	{
	public:
		void UpdateForWindowSizeChange() override { DirectXBase::UpdateForWindowSizeChange(); }
	};

	public ref class ScenarioImageSource sealed : public Windows::UI::Xaml::Media::Imaging::SurfaceImageSource
    {
	public:
		ScenarioImageSource(int pixelWidth, int pixelHeight, bool isOpaque, Platform::String^ language);

		void update();

        void BeginDraw();
        void EndDraw();

        void Clear(Windows::UI::Color color);
        void RenderNextAnimationFrame();
		void OnWindowSizeChanged(int width, int height);

		void restart();

    private protected:
	
        void OnSuspending(Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^args);
		void OnResuming(Platform::Object ^sender, Platform::Object ^args);

		void OnPointerPressed(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args);
		void OnPointerReleased(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args);

		void catchEngineError(const EngineError& e);

		AbstractMainClass* m_mainClass;
		DirectXBaseFinal m_directXBaseFinal;
		WinStoreAppControls m_winStoreAppControls;
		std::string m_crashedMessage;
	};
}

#endif
