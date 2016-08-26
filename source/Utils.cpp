#ifdef USES_WINDOWS8_METRO

#include <Windows.h>

//#include <collection.h>
//#include <ppltasks.h>

//#include "BasicReaderWriter.h"

#endif

#define ENABLE_CALLSTACK

#ifdef ENABLE_CALLSTACK
	#include "StackWalker.h"
#endif

#include <ctime>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cerrno>
#include <wchar.h>
#ifdef USES_JS_EMSCRIPTEN
	#include <emscripten.h>
	#include <cassert>
#endif
#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	#include <errno.h>
	#include <sys/time.h>
	#include <unistd.h>
#else
	#include <direct.h>
#endif

#include "../include/Utils.h"
#include "../include/FileUtil.h"
#include "EngineError.h"

#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)

const int MAX_PATH = FILENAME_MAX;

#ifdef USES_JS_EMSCRIPTEN

	void printTrace()
	{
		printf("(Call stack not available)\n");
		assert(false);
	}

#else

	#include <execinfo.h>

	void printTrace()
	{
		void *stackAddrs[100];
		size_t stackDepth = backtrace(stackAddrs, 100);
		char** stackStrings = backtrace_symbols(stackAddrs, stackDepth);
		printf("Call stack from %s:%d:\n", __FILE__, __LINE__);
		for (size_t i = 1; i < stackDepth; i++) {
			printf("    %s\n", stackStrings[i]);
		}
		free(stackStrings); // malloc()ed by backtrace_symbols
	}

#endif

#endif //USES_LINUX

namespace
{
	wchar_t s_staticPath[MAX_PATH];
	wchar_t s_pwd[MAX_PATH];
	std::wstring s_errorMessageEndUser = L"An error occured (See standard output/error stream for more informations).";

	const float FLOAT_RAND_MAX = (float)RAND_MAX;
	const double DOUBLE_RAND_MAX = (double)RAND_MAX;

	const bool THROW_ERROR = false;
}

namespace Utils
{

	//-------------------------------------------------------------------------

void setErrorMessageEndUser(const std::wstring& mess) { s_errorMessageEndUser = mess; }

//-------------------------------------------------------------------------

void die()
{
	//Utils::print("#### Program cannot continue.\n");
	//MessageBox(NULL,s_pwd,s_pwd,0);
#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	printTrace();
#else
	#ifdef USES_WINDOWS8_METRO
		throw EngineError("Error in engine");
		return;
		__debugbreak();
	#else
		#ifdef _DEBUG
				_CrtDbgBreak();
		#endif
		#ifdef ENABLE_CALLSTACK
			MyStackWalker sw;
			sw.ShowCallstack();
			std::string str = sw.getStr();
		#else
			std::string str = "ERROR (call stack infos missing)";
		#endif
		outputln(str.c_str());
		if (THROW_ERROR) throw EngineError(str);
		std::cerr << "The program was aborted, Utils::die() was called." << std::endl;
		MessageBox(NULL, s_errorMessageEndUser.c_str(), L"Fatal error - please provide this information to the developers", MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);

	#endif

	//#ifndef USES_WINDOWS8_METRO
		//	system("PAUSE");
		//#endif
#endif
	exit(1);
}

//-------------------------------------------------------------------------

void dieErrorMessageToUser(const std::string& message)
{
	dieErrorMessageToUser(Utils::convertStringToWString(message));
}

//-------------------------------------------------------------------------

void dieErrorMessageToUser(const std::wstring& message)
{
	Utils::print(L"ERROR: ");
	Utils::print(message.c_str());
	Utils::print(s_errorMessageEndUser.c_str());
	Utils::print(L"\n");
	if (THROW_ERROR) throw EngineError(message);
#if !defined(USES_LINUX) && !defined(USES_JS_EMSCRIPTEN) && !defined(USES_WINDOWS8_METRO)
	
	#ifdef ENABLE_CALLSTACK
		MyStackWalker sw;
		sw.ShowCallstack();
		std::string str = sw.getStr();
	#else
		std::string str = "ERROR (call stack infos missing)";
	#endif
	outputln(str.c_str());

	MessageBox(NULL, message.c_str(), L"Fatal error - please provide this information to the developers", MB_OK|MB_ICONEXCLAMATION);
#endif

	/*#ifndef USES_WINDOWS8_METRO
		system("PAUSE");
	#endif*/
#ifdef USES_JS_EMSCRIPTEN
	emscripten_force_exit(1);
#endif

 	exit(1);
}

//-------------------------------------------------------------------------

void dieBothErrorMessagesToUser(const std::string& message)
{
	Utils::print("ERROR: ");
	Utils::print(message.c_str());
	Utils::print(s_errorMessageEndUser.c_str());
	Utils::print("\n");
	if (THROW_ERROR) throw EngineError(message);
#if !defined(USES_LINUX) && !defined(USES_JS_EMSCRIPTEN) && !defined(USES_WINDOWS8_METRO)
	#ifdef ENABLE_CALLSTACK
		MyStackWalker sw;
		sw.ShowCallstack();
		std::string str = sw.getStr();
	#else
		std::string str = "ERROR (call stack infos missing)";
	#endif
	outputln(str.c_str());
	std::wstring mess = Utils::convertStringToWString(message) + L"\n\n" + s_errorMessageEndUser;
	MessageBox(NULL, mess.c_str(), L"Fatal error - please provide this information to the developers", MB_OK|MB_ICONEXCLAMATION);
#endif

	/*#ifndef USES_WINDOWS8_METRO
		system("PAUSE");
	#endif*/
#ifdef USES_JS_EMSCRIPTEN
	emscripten_force_exit(1);
#endif

 	exit(1);
}

//-------------------------------------------------------------------------

void warningMessageToUser(const std::wstring& message)
{
	Utils::print(L"WARNING: ");
	Utils::print(message.c_str());
	Utils::print(L"\n");
}

//-------------------------------------------------------------------------

void warningMessageToUser(const std::string& message)
{
	Utils::print("WARNING: ");
	Utils::print(message.c_str());
	Utils::print("\n");
}

//-------------------------------------------------------------------------

static FILE* fdLog = NULL;
static bool s_logMessages = false;
static std::string log_basename = "log.txt";

static int s_logIndentation = 0;

void enableMessagesInLogFile(bool yesNo) { s_logMessages = yesNo; }
void setLogFileName(const std::string& filename) { log_basename = filename; }
//std::string getLogFileFullPath() { return FileUtil::getFullPath(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str()); }
std::wstring getLogFileFullPathUnicode() { return FileUtil::getFullPathUnicode(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str()); }

void indentLog() { s_logIndentation++; }
void unindentLog() { s_logIndentation--; }
void indentLog(const char* str) { print(str);print("\n");s_logIndentation++; }
void unindentLog(const char* str) { s_logIndentation--;print(str);print("\n"); }
void resetIndentation() { s_logIndentation = 0; }

void print(const char* text)
{
	std::string finalText = "";
	for (int i = 0; i < s_logIndentation; ++i) finalText += "  ";
	finalText += text;

	std::cout << finalText << std::flush;

#if defined(USES_WINDOWS_OPENGL) || defined(USES_JS_EMSCRIPTEN) || defined(USES_WINDOWS8_DESKTOP)
	#ifdef USES_WINDOWS_OPENGL
		OutputDebugStringW(Utils::convertStringToWString(finalText).c_str());
	#endif
	if (s_logMessages)
	{
		if (fdLog == NULL)
		{
			//errno_t res = 
			_wfopen_s(&fdLog, getFullPathUnicode(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str()).c_str(), L"w");
		}

		if (fdLog != NULL)
		{
			fprintf(fdLog, "%s", finalText.c_str());
			fflush(fdLog);
		}
	}
#elif defined (USES_LINUX)
	if (s_logMessages)
	{
		if (fdLog == NULL)
		{
			fdLog = fopen(Utils::convertWStringToString(FileUtil::getFullPathUnicode(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str()), true), "w");
		}
		if (fdLog != NULL)
		{
			fprintf(fdLog, "%s", finalText.c_str());
			fflush(fdLog);
		}
	}
#elif defined(USES_WINDOWS8_METRO)
	OutputDebugStringW(Utils::convertStringToWString(std::string(finalText)).c_str());
	if (s_logMessages)
	{
		size_t size = 0;
		void* buffer = NULL;
		if (FileUtil::fileExists(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str()))
		{
			buffer = FileUtil::readFile(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str(), &size);
		}
		size_t size2 = size + finalText.size();

		unsigned char* buffer2 = new unsigned char[size2];
		if (size > 0) memcpy(buffer2, buffer, size);
		memcpy(&buffer2[size], finalText.c_str(), finalText.size());

		FileUtil::writeFile(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str(), buffer2, size2);

		delete [] buffer;
		delete [] buffer2;
	}
#endif
}

//-------------------------------------------------------------------------

void print(const wchar_t* text)
{
	//Utils::print(Utils::convertWStringToString(std::wstring(text)).c_str());return;

	std::wstring finalText = L"";
	for (int i = 0; i < s_logIndentation; ++i) finalText += L"  ";
	finalText += text;

	std::wcout << finalText << std::flush;

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN) || defined(USES_WINDOWS8_DESKTOP)
	#ifdef USES_WINDOWS_OPENGL
		OutputDebugStringW(finalText.c_str());
	#endif
	if (s_logMessages)
	{
		if (fdLog == NULL)
		{
			//errno_t res = 
			_wfopen_s(&fdLog, getFullPathUnicode(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str()).c_str(), L"w");
		}

		if (fdLog != NULL)
		{
			fwprintf(fdLog, L"%s", finalText.c_str());
			fflush(fdLog);
		}
	}
#elif defined (USES_LINUX)
	if (s_logMessages)
	{
		if (fdLog == NULL)
		{
			fdLog = fopen(Utils::convertWStringToString(FileUtil::getFullPathUnicode(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str()), true), "w");
		}
		if (fdLog != NULL)
		{
			fwprintf(fdLog, "%s", finalText.c_str());
			fflush(fdLog);
		}
	}
#elif defined(USES_WINDOWS8_METRO)
	OutputDebugStringW(finalText.c_str());
	if (s_logMessages)
	{
		size_t size = 0;
		unsigned char* buffer = NULL;
		if (FileUtil::fileExists(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str()))
		{
			buffer = FileUtil::readFile(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str(), &size);
		}
		size_t size2 = size + finalText.size()*sizeof(wchar_t);

		wchar_t* buffer2 = new wchar_t[size2];
		if (size > 0) memcpy(buffer2, buffer, size);
		memcpy(&buffer2[size], finalText.c_str(), finalText.size()*sizeof(wchar_t));

		FileUtil::writeFile(FileUtil::APPLICATION_DATA_FOLDER, log_basename.c_str(), buffer2, size2);

		delete [] buffer;
		delete [] buffer2;
	}
#endif
}

//-------------------------------------------------------------------------

void print(int number)
{
	std::stringstream sstr;
	sstr << number;
	std::string res = sstr.str();
	Utils::print(res.c_str());
}

//-------------------------------------------------------------------------

const wchar_t* getDirectoryFromFilePathStatic(const wchar_t* filename)
{
	// Store the directory path without the filename
	#if defined (USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
		wcsncpy(s_staticPath, filename, wcslen(filename));
	#else
		wchar_t* lastBSlash = wcsrchr(s_staticPath, L'\\');
		if (lastBSlash)
		{
			*(lastBSlash + 1) = L'\0';
		}
		else
		{
			*s_staticPath = L'\0';
		}
	#endif
	return s_staticPath;
}

//-------------------------------------------------------------------------

std::string dirname(std::string path)
{
	size_t pos = path.rfind('/');
	if (pos != std::string::npos)
	{
		return path.substr(0, pos);
	}
	return path;
}

//-------------------------------------------------------------------------

HANDLE checkFileHandle(HANDLE handle, const char* filenamefordebug = NULL)
{
#if defined(USES_WINDOWS_OPENGL) || defined (USES_LINUX) || defined(USES_JS_EMSCRIPTEN)

	if (handle == NULL)
	{
		if (filenamefordebug != NULL)
			dieErrorMessageToUser(std::string(filenamefordebug));
		Assert(false);
	}

#elif USES_WINDOWS8_METRO

	if (INVALID_HANDLE_VALUE == handle)
	{
		DWORD errorCode = GetLastError();
		LPTSTR message = new wchar_t[1000];
		message[0]='\0';

		FormatMessage(
			/*FORMAT_MESSAGE_ALLOCATE_BUFFER | */FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&message,
			0,
			nullptr);
		OutputDebugString(L"#### Bad handle: ");
		OutputDebugString(message);
		OutputDebugString(L"\n");
		delete message;

		Utils::die();
	}
#endif
	return handle;
}

//-------------------------------------------------------------------------

HANDLE openFile(const std::string& filepath)
{
#if defined (USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	return fopen(filepath.c_str(), "rb");
#elif defined(USES_WINDOWS_OPENGL)
	FILE* fh = NULL;
	errno_t res = fopen_s(&fh, filepath.c_str(), "rb");
	if (res != 0) die();
	return fh;
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	std::wstring wstr;
	for (size_t i = 0; i < filepath.length(); i++)
	{
		if (filepath[i] == '/') wstr += L"\\";
		else wstr += filepath[i];
	}
	HANDLE fileHandle = CreateFile2(wstr.c_str(), GENERIC_READ, 0, OPEN_EXISTING, nullptr);
	return fileHandle;
#endif
}

//-------------------------------------------------------------------------

HANDLE openFileAndCheck(const std::string& filepath)
{
	return Utils::checkFileHandle(Utils::openFile(filepath), filepath.c_str());
}

//-------------------------------------------------------------------------

bool readFile(HANDLE fileHandle, void* buffer, unsigned long int nbBytes, unsigned long int* outNbBytesRead)
{
#if defined(USES_WINDOWS_OPENGL) || defined (USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	size_t bytesRead = fread(buffer, 1, nbBytes, fileHandle);
	if (ferror(fileHandle) != 0) return false;
	if (outNbBytesRead != NULL)
		*outNbBytesRead = static_cast<unsigned long int>(bytesRead);
	return true;
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	DWORD bytesRead;
	BOOL res = ReadFile(fileHandle, buffer, nbBytes, &bytesRead, nullptr);
	*outNbBytesRead = static_cast<unsigned long int>(bytesRead);
	return res ? true : false;
#endif
}

//-------------------------------------------------------------------------

bool closeFile(HANDLE fileHandle)
{
#if defined(USES_WINDOWS_OPENGL) || defined (USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	return fclose(fileHandle) == 0;
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	BOOL res = CloseHandle(fileHandle);
	return res ? true : false;
#else
	#error
#endif
}

//-------------------------------------------------------------------------

unsigned long int getFileSize(HANDLE fileHandle)
{
#if defined(USES_WINDOWS_OPENGL) || defined (USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	long current = ftell(fileHandle);
	fseek(fileHandle, 0, SEEK_END);
	long size = ftell(fileHandle);
	fseek(fileHandle, current, SEEK_SET);
	return size;
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	// Get the file size
    FILE_STANDARD_INFO fileInfo = {0};
    if (!GetFileInformationByHandleEx(fileHandle, FileStandardInfo, &fileInfo, sizeof(fileInfo)))
    {
        throw ref new Platform::FailureException();
    }

    if (fileInfo.EndOfFile.HighPart != 0)
    {
        throw ref new Platform::OutOfMemoryException();
    }

    unsigned long int byteCount = static_cast<unsigned long int>(fileInfo.EndOfFile.LowPart);
	return byteCount;
#endif
}

//-------------------------------------------------------------------------
#if !defined(USES_LINUX) && !defined(USES_JS_EMSCRIPTEN)

void checkHResult(HRESULT hresult)
{
	if (!SUCCEEDED(hresult))
	{
		LPTSTR errorText = NULL;

		FormatMessage(
			// use system message tables to retrieve error text
			FORMAT_MESSAGE_FROM_SYSTEM
			// allocate buffer on local heap for error text
			//|FORMAT_MESSAGE_ALLOCATE_BUFFER
			// Important! will fail otherwise, since we're not
			// (and CANNOT) pass insertion parameters
			|FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
			hresult,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&errorText,  // output
			0, // minimum size for output buffer
			NULL);   // arguments - see note

		if ( NULL != errorText )
		{
			// ... do something with the string - log it, display it to the user, etc.
			outputln("#### Bad hresult: " << errorText);

			AssertMessage(false, std::wstring(L"checkHResult: Invalid hresult: ") + errorText);

			// release memory allocated by FormatMessage()
			//LocalFree(errorText);//TODO
			errorText = NULL;
		}
		else
		{
			Assert(false);
		}
	}
}
#endif
//-------------------------------------------------------------------------

wchar_t* getCurrentDirectoryStatic()
{
			//s_pwd = L"foobar";
#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	std::wstring wstr = getCurrentDirectoryUnicode();
	strcpy((char*)s_pwd, (const char*)wstr.c_str());
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
	GetCurrentDirectoryW(MAX_PATH,s_pwd);
#endif
	return s_pwd;
}

//-------------------------------------------------------------------------

/*std::string getCurrentDirectory()
{
#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	char pwdtmp[MAX_PATH];
	return std::string(getcwd(pwdtmp, MAX_PATH));
#else
	return Utils::convertWStringToString(std::wstring(getCurrentDirectoryStatic()));
#endif
}*/

//-------------------------------------------------------------------------

#ifndef USES_WINDOWS8_METRO

bool changeCurrentDirectory(const char* path)
{
#if defined(WIN32)
	return (_chdir(path) == 0);
#else
	return (chdir(path) == 0);
#endif
}
#endif

//-------------------------------------------------------------------------

std::wstring getCurrentDirectoryUnicode()
{
#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	char pwdtmp[MAX_PATH];
	return Utils::convertStringToWString(std::string(getcwd(pwdtmp, MAX_PATH)));
#else
	return std::wstring(getCurrentDirectoryStatic());
#endif
}

//-------------------------------------------------------------------------

/*void setCurrentDirectory(const std::string& newdir)
{
	#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
		if (chdir(newdir.c_str()) != 0) die();
	#else
		if(!SetCurrentDirectoryW(Utils::convertStringToWString(Utils::replaceSlashesByBackslashes(newdir)).c_str())) die();
	#endif
}*/

//-------------------------------------------------------------------------

void assertion(bool condition)
{
	if (!condition)
	{
		Utils::die();
	}
}

//-------------------------------------------------------------------------

void assertion(int line, const char* filename, bool condition)
{
	if (!condition)
	{
		std::stringstream sstr;
		sstr << "Assertion failed at line " << line << " of file " << filename;
		Utils::dieBothErrorMessagesToUser(sstr.str());
	}
}

//-------------------------------------------------------------------------

void initRandomSeed()
{
#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	srand(time(NULL));
#else
	SYSTEMTIME time;
	GetSystemTime(&time);
	srand(time.wMilliseconds);
#endif
}

//-------------------------------------------------------------------------

int random(int maxValueNotIncluded)
{
	if (maxValueNotIncluded == 0) return 0;
	return rand() % maxValueNotIncluded;
}

//-------------------------------------------------------------------------

float random(float n)
{
	return (float)rand() * n / FLOAT_RAND_MAX;
}

//-------------------------------------------------------------------------

double random(double n)
{
    return (double)rand() * n / DOUBLE_RAND_MAX;
}

//-------------------------------------------------------------------------

std::wstring convertStringToWString(std::string str)
{
	std::wstring str2(str.length(), L' ');
	std::copy(str.begin(), str.end(), str2.begin());
	return str2;
}

//-------------------------------------------------------------------------

std::string convertWStringToString(std::wstring str, bool errorIsFatal)
{
	std::string out;
	for (size_t i = 0; i < str.length(); i++)
	{
		char c[2];
		c[0] = '\0';
		c[1] = '\0';
		size_t res_str;
#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
		int res = 0;
		res_str = wcstombs(c, &str[i], 1);
#else
		errno_t res = wcstombs_s(&res_str, c, 2, &str[i], 1);

#endif
		if (res != 0)
		{
			Assert(!errorIsFatal);
			//if (res == EILSEQ) return std::string("<Could-not-convert-wstring-EILSEQ>");
			//else return std::string("<Could-not-convert-wstring>");
			out += "?";
		}
		else
		{
			//wcstombs(&c, &str[i], 1);
			out += c;
		}
	}
	return out;
}

//-------------------------------------------------------------------------

namespace
{
	// time difference between getTimeForMili_/getClockForMili_, in miliseconds
	int64_t s_timeDiff = -1;
	int64_t s_shiftForClockOverflow = 0;//every ~72 minutes, this variable should increase of ~72*60*1000

	int64_t getTimeForMili_()  { return ((int64_t)time(NULL))*1000; } // time from epoch in milisecond units but with seconds precision
	int64_t getClockForMili_() { return (((int64_t)clock())*1000/CLOCKS_PER_SEC)+s_shiftForClockOverflow; } // time in milisecond units from the program start
}

void initClock() // first run -> initialization (must be called before the 72nd minute?)
{
	Assert(s_timeDiff == -1);
	s_timeDiff = getTimeForMili_() - getClockForMili_();
}

int64_t getMillisecond()
{
	// http://stackoverflow.com/questions/3133857/c-count-time-overcoming-72-minutes-range-of-clock-t
	//
	// This hack should be useful and work whether CLOCKS_PER_SEC=1000 or CLOCKS_PER_SEC=1000000 or another value

	int64_t clo = getClockForMili_();
	int64_t timeDiff = getTimeForMili_() - clo;
	// check that clock() did not overlap.

	// If clock() value overlapped and is now too small (i.e. timeDiff bigger than expected)
	// OR:
	// If clock() value is too big (should not happen, except maybe because of an unlikely multi-thread concurrence on s_shiftForClockOverflow)
	// (Note: We use a margin of 20000 (20 seconds) to avoid overreacting, but in theory anything between 1000 (1 second) and ~4320000 (72 minutes) should work).
	if (timeDiff > s_timeDiff + 20000 || timeDiff < s_timeDiff - 20000)
	{
		// Fix s_shiftForClockOverflow to keep getClockForMili_() correct.
		s_shiftForClockOverflow += timeDiff - s_timeDiff;

		// Now call recursively to be sure that the value is now ok.
		// Also this might help in multi-thread environment, if threads are concurrent to modify s_shiftForClockOverflow value.
		return getMillisecond();
	}

    return clo;
}

//-------------------------------------------------------------------------

long long int getSecond()
{
	return time(NULL);
}

//---------------------------------------------------------------------
/**
    Renvoie la microseconde courante (depuis on sait pas quand)
*/

long long int getMicrosecondTime()
{
#if !defined(USES_LINUX) && !defined(USES_JS_EMSCRIPTEN)
    LARGE_INTEGER ticksPerSecond;
    LARGE_INTEGER tick;
    QueryPerformanceFrequency(&ticksPerSecond);
    QueryPerformanceCounter(&tick);
    //float result = (float)(tick.QuadPart % ticksPerSecond.QuadPart)*1000000.f/(float)ticksPerSecond.QuadPart;

    //return (double)tick.QuadPart*1000000.0/(double)ticksPerSecond.QuadPart;
    return (long long int)((double)tick.QuadPart*1000000.0/(double)ticksPerSecond.QuadPart);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    static long int diminutor = tv.tv_sec;
    long long int tmp = tv.tv_sec - diminutor;
    return tmp * (long long int)1000000 + (long long int)tv.tv_usec;
    //return (double)(tmp * (long long int)1000000 + (long long int)tv.tv_usec);
#endif
}

//---------------------------------------------------------------------
/**
    Do nothing and let the other program run during miliseconds time
*/

void sleepMs(unsigned int miliseconds)
{
#if defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
  usleep (miliseconds * 1000);
#elif defined(USES_WINDOWS_OPENGL)
  Sleep (miliseconds);
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	//Concurrency::wait(miliseconds);
	HANDLE m_event;
	m_event= CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
	WaitForSingleObjectEx(m_event, miliseconds, false);
#endif
}

//-------------------------------------------------------------------------

std::string replaceSlashesByBackslashes(std::string filepath)
{
	for (size_t i = 0; i < filepath.length(); i++)
	{
		if (filepath[i] == '/') filepath[i] = '\\';
	}
	return filepath;
}

//-------------------------------------------------------------------------

std::string intToString(long long int value)
{
	std::stringstream sstr;
	sstr << value;
	return sstr.str();
}

//-------------------------------------------------------------------------

std::wstring stows(std::string s)
{
	std::wstring ws;
	ws.assign(s.begin(), s.end());
	return ws;
}

std::string wstos(std::wstring ws)
{
	std::string s;
	s.assign(ws.begin(), ws.end());
	return s;
}

//-------------------------------------------------------------------------

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

	std::string platformStringToString(Platform::String^ ps) {return wstos(std::wstring(ps->Data()));}
	std::wstring platformStringToWString(Platform::String^ ps) { return std::wstring(ps->Data()); }
	Platform::String ^stringToPlatformString(std::string s) {return ref new Platform::String(stows(s).c_str());}

#endif

//-------------------------------------------------------------------------

std::vector<std::string> splitString(const std::string& str, char delim)
{
	std::vector<std::string> elems;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

//-------------------------------------------------------------------------

std::string join(const std::vector<std::string>& strs, const std::string& glue)
{
	std::string output;
	for (size_t i = 0; i < strs.size(); ++i)
	{
		output += strs[i] + (i==strs.size()-1 ? "" : glue);
	}
	return output;
}

//-------------------------------------------------------------------------

std::string trim(const std::string& str)
{
	std::string::size_type pos = str.find_last_not_of("\n \t \r");
	if(pos != std::string::npos) return str.substr(0, pos+1);
	return str;
}

//---------------------------------------------------------------------
/**
    Convertit un entier en une nouvelle chaine de caractere.
*/

char* itoa(int n)
{
    int lenstr = n == 0 ? 2 : (int)floor(log10((float)abs(n))) + 2 + (n < 0 ? 1 : 0);
    char* str = new char[lenstr];

#ifdef _MSC_VER
	sprintf_s(str, lenstr, "%d", n);
#else
	sprintf(str, "%d", n);
#endif

    return str;
}

//---------------------------------------------------------------------
/**
	Copie une partie d'une chaine dans une nouvelle chaine renvoyee.
	\param str La chaine source
	\param beginning Indice de debut dans la chaine source.
	\param end Indice de fin dans la chaine source
        (le caractere a cet indice n'est pas inclus dans la chaine renvoyee).
*/

char* copyFragmentOfStringIntoNewString(const char *str,int beginning, int end)
{
    AssertMessage(beginning < end, "copyFragmentOfStringIntoNewString: end <= beginning");

    int len = end-beginning;
    char *tmp = new char[len+1];
#ifdef _MSC_VER
	strncpy_s(tmp, len+1, &(str[beginning]), len);
#else
    strncpy(tmp,&(str[beginning]),len);
#endif
    tmp[len]='\0';

    return tmp;
}

//-------------------------------------------------------------------------

} // namespace Utils
