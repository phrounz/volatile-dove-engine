#ifndef WinStoreAppControls_h_INCLUDED
#define WinStoreAppControls_h_INCLUDED

#include "directx_inc.h"

#include <map>

class AbstractMainClass;

ref class WinStoreAppControls
{
internal:
	WinStoreAppControls() : m_semanticZoomDistance(0.f), m_isCrashedState(false) {}
	void initControls(Windows::UI::Core::CoreWindow^ window, AbstractMainClass* mainClass);
	void setCrashedState() { m_isCrashedState = true; }
private:
	
	void OnPointerPressed(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerReleased(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerMoved(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerWheelChanged(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args);
	void OnKeyDown(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::KeyEventArgs^ args);
	//void onKeyDown(int key);
	void OnKeyUp(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::KeyEventArgs^ args);
	
	std::map<unsigned int, Int2> posPointers;
	float m_semanticZoomDistance;
	AbstractMainClass* m_mainClass;
	bool m_isCrashedState;
};

#endif //WinStoreAppControls_h_INCLUDED