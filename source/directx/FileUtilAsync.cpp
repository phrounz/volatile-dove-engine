
#include <collection.h>
#include <ppltasks.h>

#include "FileUtilAsync.h"

using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;

namespace FileUtilAsync
{

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

Platform::String^ convertPathToPlatformStringAndFixSlashes(std::string str)
{
	std::wstring str2(str.length(), L' ');
	std::copy(str.begin(), str.end(), str2.begin());
	std::wstring wstr = L"";
	for (size_t i = 0; i < str2.length(); i++)
	{
		if (str2[i] == '/') wstr += L"\\";
		else wstr += str2[i];
	}
	Platform::String^ fileName = ref new Platform::String(wstr.c_str());
	return fileName;
}

//-----------------------------------------------------------------------------------

Windows::Storage::StorageFolder^ getStorageFolder(FileUtil::FileLocalization fileLocalization)
{
	if (fileLocalization == FileUtil::DOCUMENTS_FOLDER)
	{
		return Windows::Storage::KnownFolders::DocumentsLibrary;
	}
	else if (fileLocalization == FileUtil::APPLICATION_FOLDER)
	{
		auto currentPackage = Windows::ApplicationModel::Package::Current;
		return currentPackage->InstalledLocation;
	}
	else if (fileLocalization == FileUtil::APPLICATION_DATA_FOLDER)
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

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

class LReadFile
{
public:

	//-----------------------------------------------------------------------------------

	LReadFile() : m_hasFinished(false) {}

	//-----------------------------------------------------------------------------------

	void read(Windows::Storage::StorageFolder^ storageFolder, Platform::String^ parFilename, bool isInDocuments, void* (*taskToDoLater)

		(unsigned char*, size_t))
	{
		task<StorageFile^> getFileTask = task<StorageFile^>(storageFolder->GetFileAsync(parFilename));

		getFileTask.then([taskToDoLater, this](StorageFile^ storageFileSample) // Operation
		{
			create_task(FileIO::ReadBufferAsync(storageFileSample)).then([taskToDoLater, this](task<IBuffer^> task)//this,  , storageFileSample
			{
				try
				{
					IBuffer^ buffer = task.get();

					Windows::Storage::Streams::DataReader^ dreader = Windows::Storage::Streams::DataReader::FromBuffer

						(buffer);
					Platform::Array<BYTE>^ managedBytes = ref new Platform::Array<BYTE>(buffer->Length);
					dreader->ReadBytes(managedBytes);
					BYTE * bytes = new BYTE[buffer->Length];
					for (unsigned int i = 0; i < buffer->Length; i++)
						bytes[i] = managedBytes[i];
					int len = static_cast<size_t>(buffer->Length);

					//this->m_buffer = buffer;
					this->m_resultOfTaskToDoLater = taskToDoLater(bytes, len);
					this->m_hasFinished = true;
				}
				catch (Exception^ ex)
				{
					OutputDebugString(L"Pb lecture fichier.\n");
				}
			}, task_continuation_context::use_default());

		})
			.then([](task<void> t)
		{
			try
			{
				t.get();
				// .get() didn't throw, so we succeeded.
				//OutputDebugString(L"File read.\n");
			}
			catch (Platform::Exception^ e)
			{
				//Example output: The system cannot find the specified file.
				std::wstring mess = L"File not found";
				mess += L"\n";
				OutputDebugString(mess.c_str());
				__debugbreak();
			}
		});
	}

	//-----------------------------------------------------------------------------------

	bool finishedOnce()
	{
		if (m_hasFinished)
		{
			m_hasFinished = false;
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------------------

	void* getResultOfTaskToDoLater() { return m_resultOfTaskToDoLater; }

	//-----------------------------------------------------------------------------------

private:
	void* m_resultOfTaskToDoLater;
	bool m_hasFinished;
};

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

LSReadFile::LSReadFile(FileUtil::FileLocalization fileLocalization, const char* parFilename, void* (*taskToDoLater)(unsigned char*, size_t))
{
	Platform::String^ fileName = convertPathToPlatformStringAndFixSlashes(parFilename);

	m_readFile = new LReadFile();
	m_readFile->read(getStorageFolder(fileLocalization), fileName, false, taskToDoLater);
	m_taskToDoLater = taskToDoLater;
}

//-----------------------------------------------------------------------------------

LSReadFile::~LSReadFile()
{
	delete m_readFile;
}

//-----------------------------------------------------------------------------------

bool LSReadFile::finishedOnce() { return m_readFile->finishedOnce(); }

//-----------------------------------------------------------------------------------

void* LSReadFile::getResultOfTaskToDoLater() { return m_readFile->getResultOfTaskToDoLater(); }

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------


}
