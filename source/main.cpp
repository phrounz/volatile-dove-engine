
#include <vector>
#include <string>

//---------------------------------------------------------------------
#if defined(USES_LINUX)

#include "opengl/OpenGLApp.h"

#include "../include/FileUtil.h"
#include "../include/Utils.h"

const bool REDIRECT_STDOUT_AND_STDERR = false;

extern AbstractMainClass* createAbstractMainClass(const std::vector<std::string>& arguments);

int main(int argc, char* argv[])
{
	if (REDIRECT_STDOUT_AND_STDERR)
	{
		std::ofstream out("stdout.txt");
		std::cout.rdbuf(out.rdbuf());
		std::ofstream err("stderr.txt");
		std::cerr.rdbuf(err.rdbuf());
	}

	std::vector<std::string> arguments;

	for (int i = 0; i < argc; ++i)
	{
		arguments.push_back(argv[i]);
	}

	AbstractMainClass* mainClass = createAbstractMainClass(arguments);
	OpenGLApp* openGLApp = new OpenGLApp(mainClass, arguments[0].c_str());//"todo");///*lpCmdLine*/);//
	openGLApp->run();
	delete openGLApp;
	return 0;
}

//---------------------------------------------------------------------
#elif defined(USES_WINDOWS_OPENGL)

#include <Windows.h>

#include "opengl/OpenGLApp.h"

#include "../include/FileUtil.h"
#include "../include/Utils.h"

// workaround for http://stackoverflow.com/a/30498392/205768
extern "C" int _forceCRTManifestCUR = 0;

extern AbstractMainClass* createAbstractMainClass(const std::vector<std::string>& arguments);

const bool REDIRECT_STDOUT_AND_STDERR = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPredInstance, LPSTR lpCmdLine, int nShowCmd)
//int main(int argc, char* argv[])
{
	int argc = -1;

	if (REDIRECT_STDOUT_AND_STDERR)
	{
		std::ofstream out("stdout.txt");
		std::cout.rdbuf(out.rdbuf());
		std::ofstream err("stderr.txt");
		std::cerr.rdbuf(err.rdbuf());
	}

	std::vector<std::string> arguments;

	/*std::wstring lpCmdLinew = Utils::convertStringToWString(lpCmdLine);
	LPWSTR* argvw = CommandLineToArgvW(lpCmdLinew.c_str(), &argc);
	for (int i = 0; i < argc; ++i)
	{
		arguments.push_back(Utils::convertWStringToString(argvw[i]));
	}*/

	for (int i = 0; i < __argc; ++i)
	{
		arguments.push_back(__argv[i]);
	}

	AbstractMainClass* mainClass = createAbstractMainClass(arguments);
	OpenGLApp* openGLApp = new OpenGLApp(mainClass, arguments[0].c_str());//"todo");///*lpCmdLine*/);//
	openGLApp->run();
	delete openGLApp;
	return 0;
}

//---------------------------------------------------------------------
#elif defined(USES_WINDOWS8_DESKTOP)

#include "../include/Engine.h"

#include "../include/FileUtil.h"
#include "../source/directx/DesktopApp.h"

extern AbstractMainClass* createAbstractMainClass(const std::vector<std::string>& arguments);

//int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
int main(Platform::Array<Platform::String^>^ args)
{
	//TCHAR exepath[MAX_PATH + 1];
	//if (0 == GetModuleFileName(0, exepath, MAX_PATH + 1)) return 1;

	std::vector<std::string> arguments;
	for (unsigned int i = 0; i < args->Length; i++)
	{
		arguments.push_back(Utils::platformStringToString(args->Data[i]));
	}

	AbstractMainClass* mainClass = createAbstractMainClass(arguments);
	DesktopApp^ directXApp = ref new DesktopApp(mainClass);
	directXApp->Run();
	delete mainClass;
	return 0;
}

#elif defined(USES_WINDOWS8_METRO)

#ifndef USES_WINDOWS8_METRO_WITH_XAML

#include "../include/Engine.h"

#include "../source/directx/WinStoreApp.h"

extern AbstractMainClass* createAbstractMainClass(const std::vector<std::string>& arguments);

//---------------------------------------------------------------------

ref class DirectXAppSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
	{
		WinStoreApp^ res = ref new WinStoreApp;
		res->init(m_abstractMainClass);
		return res;
	}
internal:
	AbstractMainClass* getMainClass() { return m_abstractMainClass; }
	void init(AbstractMainClass* abstractMainClass) { m_abstractMainClass = abstractMainClass; }
private:
	AbstractMainClass* m_abstractMainClass;
};

//---------------------------------------------------------------------

class SimpleCrossGameLibrary
{
public:
	SimpleCrossGameLibrary(AbstractMainClass* abstractMainClass)
	{
		directXAppSource = ref new DirectXAppSource();
		directXAppSource->init(abstractMainClass);
	}
	void run()
	{
		Windows::ApplicationModel::Core::CoreApplication::Run(directXAppSource);
	}
	~SimpleCrossGameLibrary() {}
private:
	DirectXAppSource^ directXAppSource;
};

//---------------------------------------------------------------------

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^ args)
{
	//std::string argv0 = Utils::platformStringToString(*(args->begin())).c_str();//(*(argv->begin()))->Data()

	std::vector<std::string> arguments;
	for (unsigned int i = 0; i < args->Length; i++)
	{
		arguments.push_back(Utils::platformStringToString(args->Data[i]));
	}

	AbstractMainClass* mainClass = createAbstractMainClass(arguments);
	SimpleCrossGameLibrary* simpleCrossGameLibrary = new SimpleCrossGameLibrary(mainClass);
	simpleCrossGameLibrary->run();
	delete simpleCrossGameLibrary;
	delete mainClass;
    return 0;
}

#endif

//---------------------------------------------------------------------
#else
	#error
#endif
