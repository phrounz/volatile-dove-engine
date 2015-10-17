//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include <wrl.h>
#ifndef VERSION_WINDOWS_8_0
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1effects_1.h>
#include <dwrite_2.h>
#endif
#include <wincodec.h>
#include <agile.h>
#include "../../include/BasicMath.h"
#include <vector>
#include "strsafe.h"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <memory>

#include "MediaStreamer.h"

//---------------------------------------------------------------------

class StreamEffect
{
public:
	StreamEffect(const char* soundFile, bool repeat);

    // Overrides from DirectXBase
	void render();

    void onSuspending();
    void onResuming();

	void start();
	void stop();

    // Audio Functionality
	void renderAudio();

	float getVolume() const;
	void setVolume(float vol);

	// Overlay for Graphics
	~StreamEffect();

	bool isAudioStarted() const;

private:

	bool isAudioSetup();

	// Audio Related Members
    bool                            m_isAudioStarted;
    IXAudio2*                       m_musicEngine;
    IXAudio2MasteringVoice*         m_musicMasteringVoice;
    IXAudio2SourceVoice*            m_musicSourceVoice;
    std::vector<byte>               m_audioBuffers[3];
    std::unique_ptr<MediaStreamer>  m_musicStreamer;
    size_t                          m_currentBuffer;
	bool							m_hasEnded;

	bool m_repeat;
};

//---------------------------------------------------------------------
