//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include <vector>
#include "strsafe.h"
#include <memory>
#include <xaudio2.h>
#include <mmreg.h>

#include "directx_inc.h"

#include "../../include/BasicMath.h"
#include "../../include/Utils.h"
#include "RandomAccessReader.h"

RandomAccessReader::RandomAccessReader(unsigned char* buffer, size_t len): m_buffer(buffer), m_len(len)
{
	SeekToStart();
}

RandomAccessReader::~RandomAccessReader()
{
	delete [] m_buffer;
}

Platform::Array<byte>^ RandomAccessReader::Read(size_t bytesToRead)
{
	Platform::Array<byte>^ fileData = ref new Platform::Array<byte>(static_cast<uint>(bytesToRead));
	memcpy(fileData->Data, m_offset, bytesToRead);
	m_offset += bytesToRead;
	return fileData;
}

void RandomAccessReader::SeekRelative(_In_ int64 offset) {m_offset += offset;}
void RandomAccessReader::SeekAbsolute(_In_ int64 position) {m_offset = m_buffer + position;}
void RandomAccessReader::SeekToStart() {m_offset = m_buffer;}
void RandomAccessReader::SeekToEnd() {m_offset = m_buffer + m_len;}
uint64 RandomAccessReader::GetFileSize() {return m_len;}
uint64 RandomAccessReader::GetPosition() {return m_offset - m_buffer;}

/*
RandomAccessReader::RandomAccessReader(
    _In_ Platform::String^ fullPath
    )
{
	HANDLE fileHandle = Utils::openFileAndCheck(Utils::convertWStringToString(fullPath->Data()));

    m_file.Attach(fileHandle);
}

Platform::Array<byte>^ RandomAccessReader::Read(
    _In_ size_t bytesToRead
    )
{
    Platform::Array<byte>^ fileData = ref new Platform::Array<byte>(static_cast<uint>(bytesToRead));

    if (!ReadFile(
        m_file.Get(),
        fileData->Data,
        fileData->Length,
        nullptr,
        nullptr
        ) )
    {
        throw ref new Platform::FailureException();
    }

    return fileData;
}

uint64 RandomAccessReader::GetFileSize()
{
    FILE_STANDARD_INFO fileStandardInfo = { 0 };
    BOOL result = ::GetFileInformationByHandleEx(
                        m_file.Get(),
                        FileStandardInfo,
                        &fileStandardInfo,
                        sizeof(fileStandardInfo)
                        );
    if ((result == 0) || (fileStandardInfo.AllocationSize.QuadPart < 0))
    {
        throw ref new Platform::FailureException();
    }
    
    return fileStandardInfo.AllocationSize.QuadPart;
}

uint64 RandomAccessReader::GetPosition()
{
    LARGE_INTEGER position = { 0 };
    LARGE_INTEGER zero = { 0 };
    BOOL result = ::SetFilePointerEx(m_file.Get(), zero, &position, FILE_CURRENT);
    if ((result == 0) || (position.QuadPart < 0))
    {
        throw ref new Platform::FailureException();
    }
    
    return position.QuadPart;
}

void RandomAccessReader::SeekRelative(_In_ int64 offset)
{
    LARGE_INTEGER position;
    position.QuadPart = offset;
    BOOL result = ::SetFilePointerEx(m_file.Get(), position, nullptr, FILE_CURRENT);
    if (result == 0)
    {
        throw ref new Platform::FailureException();
    }
}

void RandomAccessReader::SeekAbsolute(_In_ int64 position)
{
    if (position < 0)
    {
        throw ref new Platform::FailureException();
    }

    LARGE_INTEGER pos;
    pos.QuadPart = position;
    BOOL result = ::SetFilePointerEx(m_file.Get(), pos, nullptr, FILE_BEGIN);
    if (result == 0)
    {
        throw ref new Platform::FailureException();
    }
}


void RandomAccessReader::SeekToStart()
{
    LARGE_INTEGER zero = { 0 };
    BOOL result = ::SetFilePointerEx(m_file.Get(), zero, nullptr, FILE_BEGIN);
    if (result == 0)
    {
        throw ref new Platform::FailureException();
    }
}

void RandomAccessReader::SeekToEnd()
{
    LARGE_INTEGER zero = { 0 };
    BOOL result = ::SetFilePointerEx(m_file.Get(), zero, nullptr, FILE_END);
    if (result == 0)
    {
        throw ref new Platform::FailureException();
    }
}
*/

