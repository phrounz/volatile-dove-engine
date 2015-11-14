
#include <Windows.h>
#include <string>

#ifdef USES_LINUX
	#include <sys/stat.h>
	#include <cstdlib>
#endif

#include "directx/BasicReaderWriter.h"
#include "../include/Utils.h"
#include "../include/FileUtil.h"

#include "EngineError.h"

namespace FileUtil
{
	std::wstring s_appDataFolderBasename = L"TestVideoGame";
	std::string s_currentDirVirtual = "";

	//-----------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------

	std::string convertWStringToString(std::wstring str)
	{
		std::string out;
		for (size_t i = 0; i < str.length(); i++)
		{
			char c[2];
			c[0] = '\0';
			c[1] = '\0';
#ifdef USES_LINUX
			wcstombs(c, &str[i], 1);
#else
			size_t res_str;
			errno_t res = wcstombs_s(&res_str, c, 2, &str[i], 1);
			if (res != 0) Utils::die();
#endif
			out += c;
		}
		return out;
	}

	//-----------------------------------------------------------------------------------
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	Windows::Storage::StorageFolder^ getStorageFolder(FileLocalization fileLocalization)
	{
		if (fileLocalization == DOCUMENTS_FOLDER)
		{
			return Windows::Storage::KnownFolders::DocumentsLibrary;
		}
		else if (fileLocalization == APPLICATION_FOLDER)
		{
			auto currentPackage = Windows::ApplicationModel::Package::Current;
			return currentPackage->InstalledLocation;
		}
		else if (fileLocalization == APPLICATION_DATA_FOLDER)
		{
			auto current = Windows::Storage::ApplicationData::Current;
			return current->LocalFolder;
		}
		else
		{
			__debugbreak();
			return Windows::Storage::KnownFolders::DocumentsLibrary;//for the warning
		}
	}
#endif
	//-----------------------------------------------------------------------------------
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	Platform::String^ getFilePath(const char* filepath)
	{
		return Utils::stringToPlatformString(s_currentDirVirtual + std::string(filepath));
	}
#endif
	//-----------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	Platform::Array<byte>^ readFile(FileLocalization fileLocalization, const char* filepath)
	{
		size_t size;
		void* data = readFile(fileLocalization, filepath, &size);
		Platform::Array<byte>^ bytes = ref new Platform::Array<byte>(static_cast<unsigned int>(size));
		memcpy(bytes->Data, data, size);
		delete [] data;
		return bytes;
	}
#endif
	//-----------------------------------------------------------------------------------

	void* readFile(FileLocalization fileLocalization, const char* filepath, size_t* outSize)
	{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		FILE* file = NULL;
		errno_t res = _wfopen_s(&file, getFullPathUnicode(fileLocalization, filepath).c_str(), L"rb");
		if (res != 0)
		{
			std::string mess = std::string("Missing file: \"")+filepath+'"';
			AssertMessage(false, mess.c_str());
		}
		fseek(file, 0, SEEK_END);
		size_t size = ftell(file);
		*outSize = size;
		rewind(file);
		unsigned char* data = new unsigned char[size];
		fread(data, size, 1, file);
		fclose(file);
		return data;
#elif defined(USES_WINDOWS8_METRO)
		if (!FileUtil::fileExists(fileLocalization, filepath))
		{
			std::stringstream sstr;
			sstr << "File '" << filepath << "' does not exist";
			AssertMessage(false, sstr.str().c_str());
		}
		BasicReaderWriter^ reader = ref new BasicReaderWriter(getStorageFolder(fileLocalization));
		// Load the raw texture data from disk and construct a subresource description that references it.
		auto data = reader->ReadData(getFilePath(filepath));
		*outSize = data->Length;
		unsigned char* dataCopy = new unsigned char[data->Length];
		memcpy(dataCopy, data->Data, data->Length);
		return dataCopy;
#elif defined(USES_LINUX)
		std::wstring fullpath = getFullPathUnicode(fileLocalization, filepath);
		FILE* file = fopen(Utils::convertWStringToString(fullpath).c_str(), "rb");
		fseek(file, 0, SEEK_END);
		size_t size = ftell(file);
		*outSize = size;
		rewind(file);
		unsigned char* data = new unsigned char[size];
		fread(data, size, 1, file);
		fclose(file);
		return data;
#else
	#error
#endif
	}

	//-----------------------------------------------------------------------------------

	void writeFile(FileLocalization fileLocalization, const char* filepath, const void* buffer, size_t size)
	{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		FILE* file = NULL;
		errno_t res = _wfopen_s(&file, getFullPathUnicode(fileLocalization, filepath).c_str(), L"wb");
		Assert(res == 0);
		fwrite(buffer, size, 1, file);
		fclose(file);
#elif defined(USES_WINDOWS8_METRO)
		BasicReaderWriter^ writer = ref new BasicReaderWriter(getStorageFolder(fileLocalization));

		auto bytes = ref new Platform::Array<unsigned char>((int)size);
		memcpy(bytes->Data, buffer, size);

		//Concurrency::task<void> t = writer->WriteDataAsync(getFilePath(filepath), bytes);
		//t.wait();

		try
		{
			size_t res = static_cast<size_t>(writer->WriteData(getFilePath(filepath), bytes));
			AssertRelease(res == size);
		}
		catch (Platform::FailureException^ exception)
		{
			AssertRelease(false);
		}
#elif defined(USES_LINUX)
		std::wstring fullpath = getFullPathUnicode(fileLocalization, filepath);
		FILE* file = fopen(Utils::convertWStringToString(fullpath).c_str(), "wb");
		Assert(file != NULL);
		fwrite(buffer, size, 1, file);
		fclose(file);
#else
	#error
#endif
	}

	//-----------------------------------------------------------------------------------

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
	void setAppDataFolderBasename(const wchar_t* name)
	{
		s_appDataFolderBasename = name;
	}
	std::wstring getAppDataFolderPath()
	{
		std::wstring str = L"";
		wchar_t* buf;
		size_t size;
		_wdupenv_s(&buf, &size, L"APPDATA");
		str += buf;
		str += L"\\";// TODO
		str += s_appDataFolderBasename;
		return str;
	}
#elif (defined USES_LINUX)
	void setAppDataFolderBasename(const wchar_t* name)
	{
		s_appDataFolderBasename = name;
	}
	std::wstring getAppDataFolderPath()
	{
		return Utils::convertStringToWString(std::string(getenv("HOME"))) + std::wstring(L"/") + s_appDataFolderBasename;
	}
#else
	std::wstring getAppDataFolderPath()
	{
		Windows::Storage::StorageFolder^ folder = getStorageFolder(APPLICATION_DATA_FOLDER);
		return Utils::platformStringToWString(folder->Path);
	}
#endif

	//-----------------------------------------------------------------------------------

	bool fileExists(FileLocalization fileLocalization, const char* filepath)
	{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		struct _stat st;
		return (_wstat(fullfilepath.c_str(), &st) == 0);
#elif defined(USES_WINDOWS8_METRO)
		BasicReaderWriter^ tester = ref new BasicReaderWriter(getStorageFolder(fileLocalization));
		return tester->fileExists(getFilePath(filepath));
#elif defined(USES_LINUX)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		struct stat st;
		return (stat(Utils::convertWStringToString(fullfilepath).c_str(), &st) == 0);
#else
	#error
#endif
	}

	//-----------------------------------------------------------------------------------

	bool mkdir(FileLocalization fileLocalization, const char* filepath)
	{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		return _wmkdir(fullfilepath.c_str()) == 0;
#elif defined(USES_WINDOWS8_METRO)
		Windows::Storage::StorageFolder^ folder = getStorageFolder(fileLocalization);
		std::wstring fullpath = Utils::platformStringToWString(folder->Path) + L"\\" + Utils::convertStringToWString(filepath);
		return CreateDirectoryW(fullpath.c_str(), NULL) ? true : false;
#elif defined(USES_LINUX)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		return ::mkdir(Utils::convertWStringToString(fullfilepath).c_str()) == 0;
#else
	#error
#endif
	}

	bool rmdir(FileLocalization fileLocalization, const char* filepath)
	{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		return _wrmdir(fullfilepath.c_str()) == 0;
#elif defined(USES_WINDOWS8_METRO)
		Windows::Storage::StorageFolder^ folder = getStorageFolder(fileLocalization);
		std::wstring fullpath = Utils::platformStringToWString(folder->Path) + L"\\" + Utils::convertStringToWString(filepath);
		return RemoveDirectoryW(fullpath.c_str()) ? true : false;
#elif defined(USES_LINUX)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		return ::rmdir(Utils::convertWStringToString(fullfilepath).c_str()) == 0;
#else
#error
#endif
	}

	bool deleteFile(FileLocalization fileLocalization, const char* filepath)
	{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		return _wunlink(fullfilepath.c_str()) == 0;
#elif defined(USES_WINDOWS8_METRO)
		Windows::Storage::StorageFolder^ folder = getStorageFolder(fileLocalization);
		std::wstring fullpath = Utils::platformStringToWString(folder->Path) + L"\\" + Utils::convertStringToWString(filepath);
		return DeleteFileW(fullpath.c_str()) ? true : false;
#elif defined(USES_LINUX)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		return unlink(Utils::convertWStringToString(fullfilepath).c_str()) == 0;
#else
#error
#endif
	}

	bool setCurrentDirectory(FileLocalization fileLocalization, const char* filepath)
	{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		return _wchdir(fullfilepath.c_str()) == 0;
#elif defined(USES_WINDOWS8_METRO)
		Assert(false);
		return L"";
		/*Windows::Storage::StorageFolder^ folder = getStorageFolder(fileLocalization);
		std::wstring fullpath = Utils::platformStringToWString(folder->Path) + L"\\" + Utils::convertStringToWString(filepath);
		return SetCurrentDirectoryW(fullpath.c_str());*/
#elif defined(USES_LINUX)
		std::wstring fullfilepath = getFullPathUnicode(fileLocalization, filepath);
		return chdir(Utils::convertWStringToString(fullfilepath).c_str()) == 0;
#else
#error
#endif
	}

	std::wstring getAbsoluteCurrentDirectory()
	{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		wchar_t path[MAX_PATH];
		GetCurrentDirectoryW(MAX_PATH, path);
		return std::wstring(path);
#elif defined(USES_WINDOWS8_METRO)
		Assert(false);
		return L"";
#elif defined(USES_LINUX)
		Assert(false);
		return L"";
#else
#error
#endif
	}

	//-----------------------------------------------------------------------------------

	std::wstring getFullPathUnicode(FileLocalization fileLocalization, const char* filepath)
	{
		std::wstring str;
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		if (fileLocalization == APPLICATION_FOLDER)
		{
			str += Utils::getCurrentDirectoryUnicode();
		}
		else if (fileLocalization == APPLICATION_DATA_FOLDER)
		{
			wchar_t* buf;
			size_t size;
			errno_t res = _wdupenv_s(&buf, &size, L"APPDATA");
			Assert(res == 0 && buf != NULL);
			str += buf;
			str += L"\\";// TODO
			str += s_appDataFolderBasename;
			struct _stat st;
			if (_wstat(str.c_str(), &st) != 0) // if file does not exist
			{
				_wmkdir(str.c_str());
			}
			free(buf);
		}
		else
		{
			Assert(false);// TODO
		}
#elif defined(USES_WINDOWS8_METRO)
		str = getStorageFolder(fileLocalization)->Path->Data();
#elif defined(USES_LINUX)
		if (fileLocalization == APPLICATION_FOLDER)
		{
		}
		else if (fileLocalization == APPLICATION_DATA_FOLDER)
		{
			str += getAppDataFolderPath();
		}
		else
		{
			Assert(false);// TODO
		}
#endif
		str += L"\\";
		str += Utils::convertStringToWString(filepath);
		return str;
	}

	//-----------------------------------------------------------------------------------

	std::string getFullPath(FileLocalization fileLocalization, const char* filepath)
	{
		return Utils::convertWStringToString(getFullPathUnicode(fileLocalization, filepath));
	}

	//-----------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------

	size_t getFileSize(std::ifstream& file)
	{
		if(!file.good()) return 0;
	    
		//size_t pos = file.tellg();
		file.seekg(0,std::ios::end);
		std::streamoff len = file.tellg();
		file.seekg(std::ios::beg);
	    
		return (size_t)len;
	}

	//-----------------------------------------------------------------------------------

	void copyFile(FileLocalization fileLocalizationSrc, const char* filepathSrc, FileLocalization fileLocalizationDest, const char* filepathDest)
	{
		size_t size;
		void* buffer = readFile(fileLocalizationSrc, filepathSrc, &size);
		writeFile(fileLocalizationDest, filepathDest, buffer, size);
	}


	void setCurrentDirectory(const char* path)
	{
#if defined(USES_WINDOWS8_METRO) || defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
		std::string pathstr = path;
		AssertMessage(pathstr.find("..") != std::string::npos, "setCurrentDirectory: path not supported");
		if (pathstr[pathstr.size()-1]!='/') pathstr += '/';
		s_currentDirVirtual += pathstr;
#else
		chdir(path);
#endif
	}

	//-----------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------

} //namespace FileUtil
