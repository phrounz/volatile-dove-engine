
#include "../AppSetup.h"
#include "../../include/Engine.h"
#include "../../include/AbstractMainClass.h"

#include "WinStoreAppControls.h"

using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

void WinStoreAppControls::initControls(Windows::UI::Core::CoreWindow^ window, AbstractMainClass* mainClass)
{
	m_mainClass = mainClass;

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinStoreAppControls::OnPointerPressed);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinStoreAppControls::OnPointerReleased);

	window->PointerWheelChanged +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinStoreAppControls::OnPointerWheelChanged);

	window->KeyDown +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &WinStoreAppControls::OnKeyDown);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinStoreAppControls::OnPointerMoved);

	window->KeyUp +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &WinStoreAppControls::OnKeyUp);
}

void WinStoreAppControls::OnPointerPressed(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args)
{
	if (m_isCrashedState) return;
	auto pt = args->CurrentPoint->Position;
	Int2 pos = AppSetup::instance().convertRealPositionToVirtualPosition(Int2((int)(pt.X), (int)(pt.Y)));

	posPointers[args->CurrentPoint->PointerId] = pos;

	if (posPointers.size() == 2 && m_mainClass->getSemanticZoomIsEnabled())
	{
		std::map<unsigned int, Int2>::const_iterator it = posPointers.begin();
		const Int2& pos1 = (*it).second;
		m_semanticZoomDistance = 0;
		while (true)
		{
			it++;
			if (it == posPointers.end()) break;
			m_semanticZoomDistance += pos1.distanceTo((*it).second);
		}
	}
	else
	{
		if (args->CurrentPoint->Properties->IsLeftButtonPressed)
		{
			Engine::instance().onPointerPressedInternals(MouseManager::MOUSE_LEFT_BUTTON, pos.x(), pos.y());
			m_mainClass->onPointerPressed(MouseManager::MOUSE_LEFT_BUTTON, pos.x(), pos.y());
		}
		if (args->CurrentPoint->Properties->IsMiddleButtonPressed)
		{
			Engine::instance().onPointerPressedInternals(MouseManager::MOUSE_MIDDLE_BUTTON, pos.x(), pos.y());
			m_mainClass->onPointerPressed(MouseManager::MOUSE_MIDDLE_BUTTON, pos.x(), pos.y());
		}
		if (args->CurrentPoint->Properties->IsRightButtonPressed)
		{
			Engine::instance().onPointerPressedInternals(MouseManager::MOUSE_RIGHT_BUTTON, pos.x(), pos.y());
			m_mainClass->onPointerPressed(MouseManager::MOUSE_RIGHT_BUTTON, pos.x(), pos.y());
		}
	}
}

void WinStoreAppControls::OnPointerReleased(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args)
{
	if (m_isCrashedState) return;
	auto pt = args->CurrentPoint->Position;
	Int2 pos = AppSetup::instance().convertRealPositionToVirtualPosition(Int2((int)(pt.X), (int)(pt.Y)));
	
	if (!args->CurrentPoint->Properties->IsLeftButtonPressed)
	{
		Engine::instance().onPointerReleasedInternals(MouseManager::MOUSE_LEFT_BUTTON, pos.x(), pos.y());
		m_mainClass->onPointerReleased(MouseManager::MOUSE_LEFT_BUTTON, pos.x(), pos.y());
	}
	if (!args->CurrentPoint->Properties->IsMiddleButtonPressed)
	{
		Engine::instance().onPointerReleasedInternals(MouseManager::MOUSE_MIDDLE_BUTTON, pos.x(), pos.y());
		m_mainClass->onPointerReleased(MouseManager::MOUSE_MIDDLE_BUTTON, pos.x(), pos.y());
	}
	if (!args->CurrentPoint->Properties->IsRightButtonPressed)
	{
		Engine::instance().onPointerReleasedInternals(MouseManager::MOUSE_RIGHT_BUTTON, pos.x(), pos.y());
		m_mainClass->onPointerReleased(MouseManager::MOUSE_RIGHT_BUTTON, pos.x(), pos.y());
	}

	//posPointers.erase(args->CurrentPoint->PointerId);
	posPointers.clear();
}

void WinStoreAppControls::OnPointerMoved(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args)
{
	if (m_isCrashedState) return;
	auto pt = args->CurrentPoint->Position;
	Int2 pos = AppSetup::instance().convertRealPositionToVirtualPosition(Int2((int)(pt.X), (int)(pt.Y)));

	posPointers[args->CurrentPoint->PointerId] = pos;


	if (posPointers.size() == 2 && m_mainClass->getSemanticZoomIsEnabled())
	{
		std::map<unsigned int, Int2>::const_iterator it = posPointers.begin();
		const Int2& pos1 = (*it).second;
		float semanticZoomDistance = 0;
		while (true)
		{
			it++;
			if (it == posPointers.end()) break;
			semanticZoomDistance += pos1.distanceTo((*it).second);
		}

		if (semanticZoomDistance != m_semanticZoomDistance)
		{
			m_mainClass->onSemanticZoom(semanticZoomDistance - m_semanticZoomDistance);
		}

		m_semanticZoomDistance = semanticZoomDistance;
	}
	else
	{
		bool isPressed = args->CurrentPoint->IsInContact;
		Engine::instance().onPointerMovedInternals(isPressed, pos.x(), pos.y());
		m_mainClass->onPointerMoved(isPressed, pos.x(), pos.y());
	}
}

void WinStoreAppControls::OnPointerWheelChanged(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::PointerEventArgs^ args)
{
	if (m_isCrashedState) return;
	auto pt = args->CurrentPoint->Position;
	int wheelDelta = args->CurrentPoint->Properties->MouseWheelDelta;
	Engine::instance().onPointerWheelChangedInternals(wheelDelta, (int)pt.X, (int)pt.Y);
	m_mainClass->onPointerWheelChanged(wheelDelta, (int)pt.X, (int)pt.Y);
}

void WinStoreAppControls::OnKeyDown(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::KeyEventArgs^ args)
{
	if (m_isCrashedState) return;
	int key = KeyboardManager::convertFromWin8((int)args->VirtualKey);
	Engine::instance().onKeyPressedInternals(key);
	m_mainClass->onKeyPressed(key);
}
/*
void WinStoreAppControls::onKeyDown(int key)
{
	m_mainClass->onKeyPressedInternals(key);
	m_mainClass->onKeyPressed(key);
}
*/
void WinStoreAppControls::OnKeyUp(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::KeyEventArgs^ args)
{
	if (m_isCrashedState) return;
	int key = KeyboardManager::convertFromWin8((int)args->VirtualKey);
	Engine::instance().onKeyReleasedInternals(key);
	m_mainClass->onKeyReleased(key);
}
