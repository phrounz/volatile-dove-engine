
#if defined(USES_WINDOWS_OPENGL)
	#include <al.h>
	#include <alut.h>
#else
	#include <AL/al.h>
	#include <AL/alut.h>
#endif

#include <vorbis/vorbisfile.h>

#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include "SoundLoad.h"

#include "../../include/Utils.h"

namespace SoundLoad
{

//------------------------------------------------------------------------------
// This function loads a .ogg file into a memory buffer and returns
// the format and frequency.

//#include <DataPack.h>
const int OGG_BUFFER_SIZE = 32768;// 32 KB buffers

void loadOggPrivate(const std::string& fileName, std::vector<char> &buffer, ALenum &format, ALsizei &freq)
{
    char tableBuffer[OGG_BUFFER_SIZE];
    FILE* f = fopen(fileName.c_str(), "rb");
    if (f == NULL)
    {
		std::stringstream sstr;
		sstr << "Cannot open " << fileName << " for reading...";
		outputln(sstr.str());
		Utils::dieErrorMessageToUser(sstr.str());
    }
    
    OggVorbis_File oggFile;
    if (ov_open(f, &oggFile, NULL, 0) != 0)
    {
		std::stringstream sstr;
		sstr << "Error opening ogg file " << fileName << " for decoding";
		outputln(sstr.str());
		Utils::dieErrorMessageToUser(sstr.str());
    }

    vorbis_info* pInfo = ov_info(&oggFile, -1);

    format = pInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    freq = pInfo->rate;

	int endian = 0;
    int bitStream;
    long bytes;

    do
    {
		bytes = ov_read(&oggFile, tableBuffer, OGG_BUFFER_SIZE, endian, 2, 1, &bitStream);
		if (bytes < 0)
		{
			ov_clear(&oggFile);
			std::cerr << "Error decoding " << fileName << std::endl;
			Utils::dieErrorMessageToUser("error decoding ogg file");
		}
		buffer.insert(buffer.end(), tableBuffer, tableBuffer + bytes);
    }
    while (bytes > 0);

    ov_clear(&oggFile);
}

//------------------------------------------------------------------------------

static void loadOgg(const std::string& fileName, ALuint& sourceID, ALuint& bufferID)
{

    ALenum format;                          // The sound data format
    ALsizei freq;                           // The frequency of the sound data
	std::vector<char> bufferData;                // The sound buffer data from file
    alGenBuffers(1, &bufferID);
    alGenSources(1, &sourceID);
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);
    loadOggPrivate(fileName, bufferData, format, freq);
    alBufferData(bufferID, format, &bufferData[0], static_cast<ALsizei>(bufferData.size()), freq);
}

//------------------------------------------------------------------------------

static void loadWav(const std::string& fileName, ALuint& sourceID, ALuint& bufferID)
{
	// Create sound buffer and source
	//alGenBuffers(1, &bufferID);
	alGenSources(1, &sourceID);

	/*DataPack* dataPack = DataPack::getDefaultDataPack();
	if (dataPack != NULL)
	{
		size_t fileSize;
		FILE* fileDesc = dataPack->getFileDescriptor(fileName.c_str(), fileSize, false);
		AssertMessage(fileDesc != NULL, "Sound missing in datapack");
		Assert(sizeof(char) == 1);
		char* tmpbuffer = new char[fileSize];
		fread(tmpbuffer,fileSize,1,fileDesc);

		#ifdef __APPLE__
			AssertMessage(false, "todo");
			//load sound from file
			ALsizei size;
			ALvoid *data;
			PrintValue(fileName);
			alutLoadWAVFile(4(ALbyte*)fileName,&format,&data,&size,&freq, false);
			Print("ok");
			alBufferData(bufferID, format, data, size, freq);
			alutUnloadWAV(format,data,size,freq);
			Print("ok1");
		#else
			bufferID = alutCreateBufferFromFileImage(tmpbuffer, fileSize);
		#endif

		delete [] tmpbuffer;
	}
	else
	{*/
		bufferID = alutCreateBufferFromFile(fileName.c_str());
		AssertMessage(bufferID != AL_NONE, (std::string("Loading")+fileName+": "+alutGetErrorString(alutGetError())).c_str());
	//}
}

void loadSound(const char* fileName, unsigned int* outBufferId, unsigned int* outSourceId)
{
	ALuint bufferID;            // The OpenAL sound buffer ID
	ALuint sourceID;            // The OpenAL sound source
	std::vector <char> bufferData;// The sound buffer data from file

	std::string fileNameStr = fileName;
	std::transform(fileNameStr.begin(), fileNameStr.end(), fileNameStr.begin(), tolower);
	if (fileNameStr.size()>=4 && fileNameStr.substr(fileNameStr.size()-4) == ".ogg")
	{
		loadOgg(std::string(fileName), sourceID, bufferID);
	}
	else
	{
		loadWav(std::string(fileName), sourceID, bufferID);
	}


	// Set the source and listener to the same location
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);

	// Attach sound buffer to source
	alSourcei(sourceID, AL_BUFFER, bufferID);

	if (bufferID == AL_NONE)
	{
		Utils::dieErrorMessageToUser((std::string(fileName)+": "+alutGetErrorString(alutGetError())).c_str());
	}

	*outBufferId = bufferID;
	*outSourceId = sourceID;
}

} //namespace SoundLoad
