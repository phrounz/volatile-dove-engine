//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

class RandomAccessReader
{
public:
	RandomAccessReader(unsigned char* buffer, size_t len);
	~RandomAccessReader();
	Platform::Array<byte>^ Read(size_t bytesToRead);
	void SeekRelative(int64_t offset);
	void SeekAbsolute(int64_t position);
    void SeekToStart();
    void SeekToEnd();

    uint64_t GetFileSize();
    uint64_t GetPosition();

	/*
    RandomAccessReader(_In_ Platform::String^ fullPath);

    Platform::Array<byte>^ Read(_In_ size_t bytesToRead);

    void SeekRelative(_In_ int64_t offset);
    void SeekAbsolute(_In_ int64_t position);
    void SeekToStart();
    void SeekToEnd();

    uint64_t GetFileSize();
    uint64_t GetPosition();
	*/
private:
    //Microsoft::WRL::Wrappers::FileHandle    m_file;
	unsigned char* m_buffer;
	size_t m_len;
	unsigned char* m_offset;
};
