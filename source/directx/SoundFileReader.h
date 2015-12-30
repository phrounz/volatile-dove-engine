// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include <vector>
#include "strsafe.h"
#include <memory>
#include <xaudio2.h>
#include <mmreg.h>

#include "directx_inc.h"

#include "../../include/BasicMath.h"

namespace FileUtilAsync { class LSReadFile; }

//---------------------------------------------------------------------

class SoundFileReader
{
    public:
        SoundFileReader(const char* fileName);

        Platform::Array<byte>^ GetSoundData() const;
        WAVEFORMATEX* GetSoundFormat() const;
		bool finishedOnce();

    private:
        Platform::Array<byte>^    m_soundData;
        Platform::Array<byte>^    m_soundFormat;
		FileUtilAsync::LSReadFile*   m_readFile;
};

//---------------------------------------------------------------------
