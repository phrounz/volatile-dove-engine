#ifndef FileUtil_h_INCLUDED
#define FileUtil_h_INCLUDED

#include <string>
#include <vector>
#include <fstream>

#include "Utils.h"

namespace FileUtil
{
#ifndef USES_WINDOWS8_METRO
		void setAppDataFolderBasename(const wchar_t* name);
		size_t getFileSize(std::ifstream& file);
#endif
	std::wstring getAppDataFolderPath();

	enum FileLocalization {APPLICATION_FOLDER, DOCUMENTS_FOLDER, APPLICATION_DATA_FOLDER};

	//! return file contents in a newly allocated buffer
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	Platform::Array<byte>^ readFile(FileLocalization fileLocalization, const char* filepath);
#endif
	void* readFile(FileLocalization fileLocalization, const char* filepath, size_t* outSize);
	//! write buffer on disk
	void writeFile(FileLocalization fileLocalization, const char* filepath, const void* buffer, size_t size);
	//! return true if the file exists
	bool fileExists(FileLocalization fileLocalization, const char* filepath);
	void copyFile(FileLocalization fileLocalizationSrc, const char* filepathSrc, FileLocalization fileLocalizationDest, const char* filepathDest);
	bool mkdir(FileLocalization fileLocalization, const char* filepath);
	bool rmdir(FileLocalization fileLocalization, const char* filepath);
	bool deleteFile(FileLocalization fileLocalization, const char* filepath);
	bool setCurrentDirectory(FileLocalization fileLocalization, const char* filepath);
	std::wstring getAbsoluteCurrentDirectory();

	std::string getFullPath(FileLocalization fileLocalization, const char* filepath);
	std::wstring getFullPathUnicode(FileLocalization fileLocalization, const char* filepath);

	//-----------------------------------------------------------------------------------

	inline std::string basename(std::string path)
	{
		size_t pos = path.rfind("/");
		if (pos == std::string::npos)
		{
			pos = path.rfind("\\");
			if (pos == std::string::npos) return path;
		}
		return path.substr(pos + 1);
	}

	//-----------------------------------------------------------------------------------

	inline std::string dirname(std::string path)
	{
		size_t pos = path.rfind("/");
		Assert(pos >= 0);
		std::string dirname = path.substr(0, pos);
		return dirname;
	}

	//-----------------------------------------------------------------------------------

}

#endif //FileUtil_h_INCLUDED
