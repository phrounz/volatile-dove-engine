//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "../../include/Utils.h"

#include "StreamEffect.h"
#include "DirectXHelper.h"

#ifdef USES_WINDOWS8_DESKTOP
	//#define DISABLE_AUDIO
#endif

namespace
{
	const size_t BUFFER_COUNT = 3;
};

StreamEffect::StreamEffect(const char* soundFile, bool repeat) :
    m_isAudioStarted(false),
    m_currentBuffer(0),
    m_musicEngine(nullptr),
    m_musicMasteringVoice(nullptr),
    m_musicSourceVoice(nullptr),
    m_audioBuffers(),
	m_repeat(repeat),
	m_hasEnded(false)
{
#ifdef DISABLE_AUDIO
	return;
#endif

	UINT32 flags = 0;

	outputln("Opening sound file " << soundFile);

	// Load the wave into a buffer using Media Foundation.
	// Media Foundation is a convenient way to get both file I/O and format decode for audio assets.
	// Games are welcome to replace the streamer in this sample with their own file I/O and decode routines

	Platform::String^ plstr = Utils::stringToPlatformString(std::string(soundFile));

	m_musicStreamer.reset(new MediaStreamer(plstr));
	uint32 streamSampleRate = m_musicStreamer->GetOutputWaveFormatEx().nSamplesPerSec;

	// Create XAudio2 and a single mastering voice on the default audio device

	DX::ThrowIfFailed(
		XAudio2Create(&m_musicEngine, flags)
		);

	// This sample plays the equivalent of background music, which we tag on the mastering voice as AudioCategory_GameMedia.
	// In ordinary usage, if we were playing the music track with no effects, we could route it entirely through
	// Media Foundation. Here we are using XAudio2 to apply a reverb effect to the music, so we use Media Foundation to
	// decode the data then we feed it through the XAudio2 pipeline as a separate Mastering Voice, so that we can tag it
	// as Game Media.

	HRESULT hr = m_musicEngine->CreateMasteringVoice(
		&m_musicMasteringVoice,
		XAUDIO2_DEFAULT_CHANNELS,
		streamSampleRate,
		0,
		nullptr,    // Use the default audio device
		nullptr,    // No effect chain on the mastering voice
		AudioCategory_SoundEffects //AudioCategory_GameMedia
		);
	if (FAILED(hr))
	{
		//
		// Unable to create the mastering voice
		// This happens if there are no audio devices
		//
		m_musicMasteringVoice = nullptr;
		return;
	}

	// Set up Reverb effect
	
	const WAVEFORMATEX& waveFormat = m_musicStreamer->GetOutputWaveFormatEx();

	DX::ThrowIfFailed(
		m_musicEngine->CreateSourceVoice(&m_musicSourceVoice, &waveFormat, 0, 1.0f, nullptr, nullptr, nullptr)
		);
	/*
	Microsoft::WRL::ComPtr<IUnknown> pXAPO;
	DX::ThrowIfFailed(
		XAudio2CreateReverb(&pXAPO)
		);
	XAUDIO2_EFFECT_DESCRIPTOR descriptor;
	descriptor.InitialState = true;
	descriptor.OutputChannels = waveFormat.nChannels;
	descriptor.pEffect = pXAPO.Get();
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descriptor;

	// Create the source voice and start it
	DX::ThrowIfFailed(
		m_musicEngine->CreateSourceVoice(&m_musicSourceVoice, &waveFormat, 0, 1.0f, nullptr, nullptr, &chain)
		);
	
	XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
	reverbParameters.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
	reverbParameters.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
	reverbParameters.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
	reverbParameters.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
	reverbParameters.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
	reverbParameters.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
	reverbParameters.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
	reverbParameters.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
	reverbParameters.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
	reverbParameters.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
	reverbParameters.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
	reverbParameters.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
	reverbParameters.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
	reverbParameters.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
	reverbParameters.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
	reverbParameters.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
	reverbParameters.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
	reverbParameters.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;
	reverbParameters.DisableLateField = FALSE;
	
	DX::ThrowIfFailed(
		m_musicSourceVoice->SetEffectParameters(0, &reverbParameters, sizeof(reverbParameters))
		);
		
	DX::ThrowIfFailed(
		m_musicSourceVoice->EnableEffect(0)
		);*/
}

StreamEffect::~StreamEffect()
{
#ifdef DISABLE_AUDIO
	return;
#endif
	outputln("Closing sound file");

	if (m_musicSourceVoice != nullptr)
	{
		m_musicSourceVoice->DestroyVoice();
		m_musicSourceVoice = nullptr;
	}
	if (m_musicMasteringVoice != nullptr)
	{
		m_musicMasteringVoice->DestroyVoice();
		m_musicMasteringVoice = nullptr;
	}
	if (m_musicEngine != nullptr)
	{
		m_musicEngine->Release();
		m_musicEngine = nullptr;
	}
}

void StreamEffect::render()
{
#ifdef DISABLE_AUDIO
	return;
#endif
	if (isAudioSetup())
    {
		renderAudio();
    }
    else
    {
        // No audio, render a message indicating setup failure
        Assert(false);
    }
}

void StreamEffect::onSuspending()
{
#ifdef DISABLE_AUDIO
	return;
#endif
    m_musicEngine->StopEngine();
}

void StreamEffect::onResuming()
{
#ifdef DISABLE_AUDIO
	return;
#endif
	DX::ThrowIfFailed(
        m_musicEngine->StartEngine()
        );
}

float StreamEffect::getVolume() const
{
#ifdef DISABLE_AUDIO
	return 0.f;
#endif
	float vol;
	m_musicSourceVoice->GetVolume(&vol);
	return vol;
}

void StreamEffect::setVolume(float vol)
{
#ifdef DISABLE_AUDIO
	return;
#endif
	m_musicSourceVoice->SetVolume(vol);
}

void StreamEffect::start()
{
#ifdef DISABLE_AUDIO
	return;
#endif
	if (!m_isAudioStarted && (m_musicSourceVoice != nullptr))
	{
		//m_currentBuffer = 0;
		DX::ThrowIfFailed(
			m_musicSourceVoice->Start(0)
			);

		m_hasEnded = false;
		m_isAudioStarted = true;
	}
}

void StreamEffect::stop()
{
#ifdef DISABLE_AUDIO
	return;
#endif
	if (m_isAudioStarted && (m_musicSourceVoice != nullptr))
	{
		DX::ThrowIfFailed(
			m_musicSourceVoice->Stop()
			);

		m_hasEnded = true;
		m_isAudioStarted = false;
	}
}

// This sample processes audio buffers during the render cycle of the application.
// As long as the sample maintains a high-enough frame rate, this approach should
// not glitch audio. In game code, audio buffers should be processed on a separate thread
// that is not synched to the main render loop of the game.

void StreamEffect::renderAudio()
{
#ifdef DISABLE_AUDIO
	return;
#endif
	if (isAudioSetup() == false)
    {
        return;
    }

	if (m_hasEnded)
	{
		// Find the current state of the playing buffers
		XAUDIO2_VOICE_STATE state;
		m_musicSourceVoice->GetState(&state);

		// Have all of the buffer completed
		if (state.BuffersQueued == 0)
		{
			this->stop();
		}
		return;
	}

    // Find the current state of the playing buffers
    XAUDIO2_VOICE_STATE state;
    m_musicSourceVoice->GetState(&state);

    // Have any of the buffer completed
    while (state.BuffersQueued < BUFFER_COUNT)
    {
        // Retreive the next buffer to stream from MF Music Streamer
        m_audioBuffers[m_currentBuffer] = m_musicStreamer->GetNextBuffer();
        if (m_audioBuffers[m_currentBuffer].size() == 0)
        {
			// Audio file has been fully read, restart the reader to re-loop the audio
			m_musicStreamer->Restart();
			if (!m_repeat)
			{
				m_hasEnded = true;
				//this->stop();
			}
			break;
        }

        //
        // Submit the new buffer
        //
        XAUDIO2_BUFFER buf = {0};
        buf.AudioBytes = static_cast<uint32>(m_audioBuffers[m_currentBuffer].size());
        buf.pAudioData = &m_audioBuffers[m_currentBuffer][0];
        DX::ThrowIfFailed(
            m_musicSourceVoice->SubmitSourceBuffer(&buf)
            );

        // Advance the buffer index
		m_currentBuffer = ++m_currentBuffer % BUFFER_COUNT;

        // Get the updated state
        m_musicSourceVoice->GetState(&state);
    }
}

bool StreamEffect::isAudioSetup()
{
#ifdef DISABLE_AUDIO
	return true;
#endif
    return ((m_musicEngine != nullptr) &&
            (m_musicMasteringVoice != nullptr) &&
            (m_musicSourceVoice != nullptr)
            );
}

bool StreamEffect::isAudioStarted() const
{
#ifdef DISABLE_AUDIO
	return false;
#endif
	return m_isAudioStarted;
}

/*
++m_currentBuffer;
if (m_currentBuffer >= BUFFER_COUNT)
{
	this->stop();
	if (!m_repeat)
	{
	m_currentBuffer = 0;
	m_isAudioStarted = false;
	break;
	}
	m_currentBuffer = 0;
}
*/