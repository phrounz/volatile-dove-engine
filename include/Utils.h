#ifndef Utils_h_INCLUDED
#define Utils_h_INCLUDED

#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>

#ifdef USES_WINDOWS_OPENGL
	#include "stdint.h"
#else
	#include <stdint.h>
#endif

#if defined(USES_WINDOWS8_METRO) || defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS_OPENGL)
	#include <Windows.h>
#endif

namespace Utils
{
	void setErrorMessageEndUser(const std::wstring& mess);
	void die();
	void assertion(bool condition);
	void assertion(int line, const char* filename, bool condition);
	void dieErrorMessageToUser(const std::string& message);
	void dieErrorMessageToUser(const std::wstring& message);
	void warningMessageToUser(const std::string& message);
	void warningMessageToUser(const std::wstring& message);

	void enableMessagesInLogFile(bool yesNo);
	void setLogFileName(const std::string& filename);
	//std::string getLogFileFullPath();
	std::wstring getLogFileFullPathUnicode();

	void indentLog();
	void unindentLog();
	void indentLog(const char* str);
	void unindentLog(const char* str);
	void resetLogIndentation();

	void print(const char* text);
	void print(const wchar_t* text);
	void print(int number);
	
	std::wstring convertStringToWString(std::string str);
	std::string convertWStringToString(std::wstring str, bool errorIsFatal);

	#if !defined(USES_LINUX) && !defined(USES_JS_EMSCRIPTEN)
		void checkHResult(HRESULT hresult);
	#endif

#if !defined(USES_WINDOWS8_DESKTOP) && !defined(USES_WINDOWS8_METRO)
	#define HANDLE FILE*
#endif
	HANDLE openFile(const std::string& filepath);
	HANDLE openFileAndCheck(const std::string& filepath);
	bool readFile(HANDLE fileHandle, void* buffer, unsigned long int nbBytes, unsigned long int* outNbBytesRead);
	bool closeFile(HANDLE fileHandle);
	unsigned long int getFileSize(HANDLE fileHandle);

	const wchar_t* getDirectoryFromFilePathStatic(const wchar_t* filename);//TODO check

	wchar_t* getCurrentDirectoryStatic();//TODO check
	//std::string getCurrentDirectory();
	std::wstring getCurrentDirectoryUnicode();
#ifndef USES_WINDOWS8_METRO
	bool changeCurrentDirectory(const char* path);
#endif
	//void setCurrentDirectory(const std::string& newdir);

	void initRandomSeed();
	int random(int maxValueNotIncluded);
	float random(float n);
	double random(double n);
	void initClock();//must only be called once at startup
	int64_t getMillisecond();
	long long int getSecond();

	long long int getMicrosecondTime();
	void sleepMs(unsigned int miliseconds);

	std::string replaceSlashesByBackslashes(std::string filepath);
	std::string intToString(long long int value);

	#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
		std::string platformStringToString(Platform::String^ ps);
		std::wstring platformStringToWString(Platform::String^ ps);
		Platform::String ^stringToPlatformString(std::string s);
	#endif

	std::string dirname(std::string path);

	std::vector<std::string> splitString(const std::string& str, char delim);
	std::string join(const std::vector<std::string>& strs, const std::string& glue);
	std::string trim(const std::string& str);
	char* itoa(int n);
	char* copyFragmentOfStringIntoNewString(const char *str,int beginning, int end);
}

#define output( a ) {std::stringstream sstr;sstr << a ;Utils::print(sstr.str().c_str());}
#define outputln( a ) {std::stringstream sstr;sstr << a << std::endl;Utils::print(sstr.str().c_str());}//Utils::print((sstr.str() + "\n").c_str());}
#define outputwln( a ) {std::wstringstream sstr;sstr << a << std::endl;Utils::print(sstr.str().c_str());}//Utils::print((sstr.str() + "\n").c_str());}
/*#ifdef USES_LINUX
  #define Assert( a ) {if ( ! (a) ) {std::stringstream sstr;sstr << "ASSERTION FAILED: " << __FILE__ << ":" << __LINE__ <<"\n";Utils::print(sstr.str().c_str());exit(1);}}
#else*/
  #ifdef _DEBUG
    //#define Assert( a )
    #define Assert( a ) Utils::assertion(__LINE__, __FILE__, ( a ) )
	#define AssertMessage( a , b ) {if ( ! (a) ) {Utils::dieErrorMessageToUser( b );}}
  #else
	//#define Assert( a )
	#ifdef USES_WINDOWS8_METRO
		#define Assert( a ) Utils::assertion(__LINE__, __FILE__, ( a ) )
	#else
		//#define Assert Utils::assertion
		#define Assert( a ) Utils::assertion(__LINE__, __FILE__, ( a ) )
	#endif
	#define AssertMessage( a , b ) {if ( ! (a) ) {Utils::dieErrorMessageToUser( b );}}
  #endif
//#endif

#define AssertRelease Utils::assertion

#define iterateVector(x, y) for(std::vector<y>::iterator it = (x).begin(); it != (x).end(); it++)
#define iterateVector2(x, y) for(std::vector<y>::iterator it2 = (x).begin(); it2 != (x).end(); it2++)
#define iterateVectorConst(x, y) for(std::vector<y>::const_iterator it = (x).begin(); it != (x).end(); it++)
#define iterateVectorConst2(x, y) for(std::vector<y>::const_iterator it2 = (x).begin(); it2 != (x).end(); it2++)
#define iterateMap(x, y, z) for(std::map<y,z>::iterator it = (x).begin(); it != (x).end(); it++)
#define iterateMapConst(x, y, z) for(std::map<y,z>::const_iterator it = (x).begin(); it != (x).end(); it++)
#define iterateMapConst2(x, y, z) for(std::map<y,z>::const_iterator it2 = (x).begin(); it2 != (x).end(); it2++)
#define iterateSet(x, y) for(std::set<y>::iterator it = (x).begin(); it != (x).end(); it++)
#define iterateSetConst(x, y) for(std::set<y>::const_iterator it = (x).begin(); it != (x).end(); it++)
#define iterateSetConst2(x, y) for(std::set<y>::const_iterator it2 = (x).begin(); it2 != (x).end(); it2++)
#define iterateList(x, y) for(std::list<y>::iterator it = (x).begin(); it != (x).end(); it++)
#define iterateList2(x, y) for(std::list<y>::iterator it2 = (x).begin(); it2 != (x).end(); it2++)
#define iterateListConst(x, y) for(std::list<y>::const_iterator it = (x).begin(); it != (x).end(); it++)
#define iterateListConst2(x, y) for(std::list<y>::const_iterator it2 = (x).begin(); it2 != (x).end(); it2++)
#define iterateMultimap(x, y, z) for(std::multimap<y,z>::iterator it = (x).begin(); it != (x).end(); it++)
#define iterateMultimapConst(x, y, z) for(std::multimap<y,z>::const_iterator it = (x).begin(); it != (x).end(); it++)
#define reverseIterateMultimapConst(x, y, z) for(std::multimap<y,z>::const_reverse_iterator it = (x).rbegin(); it != (x).rend(); it++)
/*
#define benchmarkInit(x) static std::map<int,std::vector<i64> mapTime;std::vector<i64> tmpTime;tmpTime.push_back(Utils::getMicrosecondTime());std::vector<std::string> tmpCaption;tmpCaption.push_back( x );int iBench=0;
#define benchmarkPoint(x) {i64 ms = Utils::getMicrosecondTime();mapTime[iBench].push_back(ms);tmpTime.push_back(ms);tmpCaption.push_back( x );++iBench;}
#define benchmarkConclude() { outputln("==" << tmpCaption[0]);for (size_t iz = 1; iz < tmpTime.size(); ++iz) outputln("benchmark: " << tmpCaption[iz] << ": " << (tmpTime[iz] - tmpTime[iz - 1])); for (size_t ib=0;ib<mapTime.size();++ib) {for (size_t ic=0;ic<mapTime[ib].second.size();++ic) { mapTime[ib].second[ic]  TODO } } }
*/
#define benchmarkInit(x) std::vector<i64> tmpTime;tmpTime.push_back(Utils::getMicrosecondTime());std::vector<std::string> tmpCaption;tmpCaption.push_back( x );int iBench=0;
#define benchmarkPoint(x) {i64 ms = Utils::getMicrosecondTime();tmpTime.push_back(ms);tmpCaption.push_back( x );++iBench;}
#define benchmarkConclude() { outputln("==" << tmpCaption[0]);for (size_t iz = 1; iz < tmpTime.size(); ++iz) outputln("benchmark: " << tmpCaption[iz] << ": " << (tmpTime[iz] - tmpTime[iz - 1])); }

#endif //Utils_h_INCLUDED
