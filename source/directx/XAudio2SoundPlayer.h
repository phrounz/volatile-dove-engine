//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include <wrl.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include <vector>
#include "strsafe.h"
#include <memory>
#include <xaudio2.h>
#include <mmreg.h>

//#include "DirectXBase.h"

#include "mmreg.h"
#include <vector>
#include <memory>

#include "../../include/BasicMath.h"

class XAudio2SoundPlayer
{
    struct ImplData;

    public:
        XAudio2SoundPlayer(_In_ uint32 sampleRate);
        ~XAudio2SoundPlayer();

        size_t AddSound(_In_ WAVEFORMATEX* format, _In_ Platform::Array<byte>^ data);
        bool   PlaySound(_In_ size_t index);
        bool   StopSound(_In_ size_t index);
        bool   IsSoundPlaying(_In_ size_t index) const;
        size_t GetSoundCount() const;

        void Suspend();
        void Resume();

    private:
        interface IXAudio2*                     m_audioEngine;
        interface IXAudio2MasteringVoice*       m_masteringVoice;
        std::vector<std::shared_ptr<ImplData>>  m_soundList;
};

