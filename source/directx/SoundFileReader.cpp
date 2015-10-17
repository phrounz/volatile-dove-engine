//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED FileUtilOR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "FileUtilAsync.h"

#include "SoundFileReader.h"
#include "RandomAccessReader.h"

namespace
{
    //
    // 4 Character Tags in the RIFF File of Interest (read backwards)
    //
    const uint32_t FOURCC_RIFF_TAG      = 'FFIR';
	const uint32_t FOURCC_FORMAT_TAG = ' tmf';
	const uint32_t FOURCC_DATA_TAG = 'atad';
	const uint32_t FOURCC_WAVE_FILE_TAG = 'EVAW';

    //
    // The header of every 'chunk' of data in the RIFF file
    //
    struct ChunkHeader
    {
        uint32_t tag;
        uint32_t size;
    };

    //
    // Helper function to find a riff-chunk with-in the sent bounds.
    // It is assumed the bounds begin at the start of a chunk
    //
    uint64_t FindChunk(
                RandomAccessReader* file,
                uint32_t tag,
                uint64_t startLoc,
                uint64_t endLoc
                )
    {
        // Move to start of search
        file->SeekAbsolute(startLoc);

        uint64_t newLoc = startLoc;
        while (endLoc > (newLoc + sizeof(ChunkHeader)))
        {
            Platform::Array<byte>^ headerBytes = file->Read(sizeof(ChunkHeader));
            ChunkHeader* header = reinterpret_cast<ChunkHeader*>(headerBytes->Data);
            if (header->tag == tag)
            {
                // Found the requested tag
                return newLoc;
            }
            file->SeekRelative(static_cast<int64>(header->size));
            newLoc += header->size + sizeof(*header);
        }

        // Chunk with sent tag was not found
        throw ref new Platform::FailureException();
    }

    //
    // Read the riff chunk header at the send location
    //
    void ReadHeader(
            RandomAccessReader* file,
            uint64_t loc,
            ChunkHeader& header
            )
    {
        // Move to read location
        file->SeekAbsolute(loc);
        Platform::Array<byte>^ headerBytes = file->Read(sizeof(ChunkHeader));
        header = *reinterpret_cast<ChunkHeader*>(headerBytes->Data);
    }
}

struct SoundDataTmp
{
	Platform::Array<byte>^	  m_soundData;
	Platform::Array<byte>^    m_soundFormat;
};

void* loadSound(unsigned char* buffer, size_t len)
{	
	SoundDataTmp* tmp = new SoundDataTmp;

	//
    // Open the file for random read access
    //
	//RandomAccessReader* riffFile = new RandomAccessReader(fileName);
	RandomAccessReader* riffFile = new RandomAccessReader(buffer, len);
    uint64_t fileSize = riffFile->GetFileSize();

    //
    // Locate, read and validate the RIFF chunk
    //
    uint64_t riffLoc = FindChunk(riffFile, FOURCC_RIFF_TAG, 0, fileSize);

    // Read beyond the riff header.
    ChunkHeader chunkHeader;
    ReadHeader(riffFile, riffLoc, chunkHeader);

    uint32_t tag = 0;
    Platform::Array<byte>^ riffData = riffFile->Read(sizeof(tag));
    tag = *reinterpret_cast<uint32_t*>(riffData->Data);
    if (tag != FOURCC_WAVE_FILE_TAG)
    {
        // Only support .wav files
        throw ref new Platform::FailureException();
    }
    uint64_t riffChildrenStart = riffLoc + sizeof(chunkHeader) + sizeof(tag);
    uint64_t riffChildrenEnd   = riffLoc + sizeof(chunkHeader) + chunkHeader.size;

    //
    // Find, read and validate the format chunk (a child within the RIFF chunk)
    //
    uint64_t formatLoc = FindChunk(riffFile, FOURCC_FORMAT_TAG, riffChildrenStart, riffChildrenEnd);
    ReadHeader(riffFile, formatLoc, chunkHeader);
    if (chunkHeader.size < sizeof(WAVEFORMATEX))
    {
        // Format data of unsupported size; must be unsupported format
        throw ref new Platform::FailureException();
    }
    tmp->m_soundFormat = riffFile->Read(chunkHeader.size);
    WAVEFORMATEX format = *reinterpret_cast<WAVEFORMATEX*>(tmp->m_soundFormat->Data);
    if (format.wFormatTag != WAVE_FORMAT_PCM
        && format.wFormatTag != WAVE_FORMAT_ADPCM)
    {
        // This is not PCM or ADPCM, which is all we support
        throw ref new Platform::FailureException();
    }

    //
    // Locate, the PCM data in the data chunk
    //
    uint64_t dataChunkStart = FindChunk(riffFile, FOURCC_DATA_TAG, riffChildrenStart, riffChildrenEnd);
    ReadHeader(riffFile, dataChunkStart, chunkHeader);

    //
    // Now read the PCM data and setup the buffer
    //
    tmp->m_soundData = riffFile->Read(chunkHeader.size);

	delete riffFile;

	return tmp;
}

//--------------------------------------------------------------------------------------
// Name: SoundFileReader constructor
// Desc: Any failure to construct will throw.
//       If the constructor succeeds this is a fully usable object
//--------------------------------------------------------------------------------------
SoundFileReader::SoundFileReader(const char* fileName)
{
	m_readFile = new FileUtilAsync::LSReadFile(FileUtil::APPLICATION_FOLDER, fileName, &loadSound);
	/*
    //
    // Open the file for random read access
    //
	RandomAccessReader* riffFile = new RandomAccessReader(fileName);
    uint64_t fileSize = riffFile->GetFileSize();

    //
    // Locate, read and validate the RIFF chunk
    //
    uint64_t riffLoc = FindChunk(riffFile, FOURCC_RIFF_TAG, 0, fileSize);

    // Read beyond the riff header.
    ChunkHeader chunkHeader;
    ReadHeader(riffFile, riffLoc, chunkHeader);

    uint32_t tag = 0;
    Platform::Array<byte>^ riffData = riffFile->Read(sizeof(tag));
    tag = *reinterpret_cast<uint32_t*>(riffData->Data);
    if (tag != FOURCC_WAVE_FILE_TAG)
    {
        // Only support .wav files
        throw ref new Platform::FailureException();
    }
    uint64_t riffChildrenStart = riffLoc + sizeof(chunkHeader) + sizeof(tag);
    uint64_t riffChildrenEnd   = riffLoc + sizeof(chunkHeader) + chunkHeader.size;

    //
    // Find, read and validate the format chunk (a child within the RIFF chunk)
    //
    uint64_t formatLoc = FindChunk(riffFile, FOURCC_FORMAT_TAG, riffChildrenStart, riffChildrenEnd);
    ReadHeader(riffFile, formatLoc, chunkHeader);
    if (chunkHeader.size < sizeof(WAVEFORMATEX))
    {
        // Format data of unsupported size; must be unsupported format
        throw ref new Platform::FailureException();
    }
    m_soundFormat = riffFile->Read(chunkHeader.size);
    WAVEFORMATEX format = *reinterpret_cast<WAVEFORMATEX*>(m_soundFormat->Data);
    if (format.wFormatTag != WAVE_FORMAT_PCM
        && format.wFormatTag != WAVE_FORMAT_ADPCM)
    {
        // This is not PCM or ADPCM, which is all we support
        throw ref new Platform::FailureException();
    }

    //
    // Locate, the PCM data in the data chunk
    //
    uint64_t dataChunkStart = FindChunk(riffFile, FOURCC_DATA_TAG, riffChildrenStart, riffChildrenEnd);
    ReadHeader(riffFile, dataChunkStart, chunkHeader);

    //
    // Now read the PCM data and setup the buffer
    //
    m_soundData = riffFile->Read(chunkHeader.size);

	delete riffFile;
	*/
}

//--------------------------------------------------------------------------------------
// Name: SoundFileReader::GetSoundData
// Desc: Sound Data accessor
//--------------------------------------------------------------------------------------
Platform::Array<byte>^ SoundFileReader::GetSoundData() const
{
    return m_soundData;
}

//--------------------------------------------------------------------------------------
// Name: SoundFileReader::GetSoundFormat
// Desc: Sound Format Accessor
//--------------------------------------------------------------------------------------
WAVEFORMATEX* SoundFileReader::GetSoundFormat() const
{
    return reinterpret_cast<WAVEFORMATEX*>(m_soundFormat->Data);
}

bool SoundFileReader::finishedOnce()
{
	if (m_readFile->finishedOnce())
	{
		SoundDataTmp* soundDataTmp = reinterpret_cast<SoundDataTmp*>(m_readFile->getResultOfTaskToDoLater());
		m_soundFormat = soundDataTmp->m_soundFormat;
		m_soundData = soundDataTmp->m_soundData;
		delete soundDataTmp;
		return true;
	}
	return false;
}
