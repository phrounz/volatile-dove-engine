#pragma once

#include "../../include/FileUtil.h"

//---------------------------------------------------------------------

namespace FileUtilAsync
{
	class LReadFile;

	class LSReadFile
	{
	public:
		LSReadFile(FileUtil::FileLocalization fileLocalization, const char* parFilename, void* (*taskToDoLater)(unsigned char*, size_t));
		~LSReadFile();
		bool finishedOnce();
		void* getResultOfTaskToDoLater();
	private:
		LReadFile* m_readFile;
		void* (*m_taskToDoLater)(unsigned char*, size_t);
	};

}

//---------------------------------------------------------------------
