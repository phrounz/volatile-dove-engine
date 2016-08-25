
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

#include <ppltasks.h>

#include "../include/Utils.h"
#include "../include/Thread.h"

namespace Threading
{
	class ThreadPriv
	{
	public:
		ThreadPriv() {} //:m_hasFinished(false),m_resultOfTask(NULL) 
		void* param;
		void*(*func)(void*);
		//void* m_resultOfTask;
		//bool m_hasFinished;
	};

	//------------------------------------------------

	void Thread::createThread(void *(*startRoutine)(void *), void *startRoutineArg)
	{
		Thread* thread = new Thread(startRoutine, startRoutineArg);
#pragma message("TODO Thread::createThread")
	}

	//------------------------------------------------

	Thread::Thread(void*(*funcP)(void*), void* paramP)
	{
		ThreadPriv io;
		io.func = funcP;
		io.param = paramP;
		//io->m_hasFinished = false;

		Concurrency::create_task([io]
		{
			try
			{
				(*io.func)(io.param);//io->m_resultOfTask = 
				//io->m_hasFinished = true;
			}
			catch (Platform::Exception^ ex)
			{
				__debugbreak();
			}
		});
	}

	/*
	void* Thread::get()
	{
	while(!priv->m_hasFinished) {}
	return priv->m_resultOfTask;
	}
	*/

	void Thread::initSemaphore()
	{
	}

	void Thread::claimSemaphore()
	{
		Assert(false);// TODO
	}

	void Thread::releaseSemaphore()
	{
		Assert(false);// TODO
	}

	void Thread::deinitSemaphore()
	{
	}

	Thread::~Thread()
	{
		delete this->priv;
	}
}

#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	
#include <pthread.h>
#include <semaphore.h>

#if defined(USES_WINDOWS_OPENGL)
    #include <windows.h>
    #include <winbase.h>
#else
    #include <unistd.h>
#endif

#include "../include/Utils.h"

#include "../include/Thread.h"

namespace Threading
{
	//------------------------------------------------

	class ThreadPriv
	{
	public:
		void* argThreadV;
		void* pThreadV;
	};

	volatile sem_t mutex;

	//------------------------------------------------

	void Thread::createThread(void *(*startRoutine)(void *),void *startRoutineArg)
	{
		Thread* thread = new Thread(startRoutine, startRoutineArg);
		delete (pthread_attr_t*)thread->priv->argThreadV;
		delete (pthread_t*)thread->priv->pThreadV;
	}

	//------------------------------------------------

	Thread::Thread(void *(*startRoutine)(void *),void *startRoutineArg)
	{
		this->priv = new ThreadPriv;

		pthread_attr_t* argThread = new pthread_attr_t();
		pthread_t* pThread = new pthread_t();

		//configure le nouveau thread
		int ret1 = pthread_attr_init(argThread);
		AssertMessage(ret1 != -1, "error during arguments initialisation (pthread_attr).");

		//lance le nouveau thread
		int ret2 = pthread_create(pThread,argThread,startRoutine,startRoutineArg);
		AssertMessage(ret2 != -1, "error during the creation of the new thread.");

		this->priv->argThreadV = argThread;
		this->priv->pThreadV = pThread;
	}

	//------------------------------------------------

	void Thread::initSemaphore()
	{
		// http://stackoverflow.com/questions/5249895/how-to-cast-away-the-volatile-ness
		sem_init(const_cast<sem_t*>(&mutex), 0, 1);
	}

	//------------------------------------------------

	void Thread::claimSemaphore()
	{
		sem_wait(const_cast<sem_t*>(&mutex));
	}

	//------------------------------------------------

	void Thread::releaseSemaphore()
	{
		sem_post(const_cast<sem_t*>(&mutex));
	}

	//------------------------------------------------

	void Thread::deinitSemaphore()
	{
		sem_destroy(const_cast<sem_t*>(&mutex));
	}

	//------------------------------------------------

	Thread::~Thread()
	{
		void* val;
		pthread_join(*(pthread_t*)priv->pThreadV, &val);
		delete (pthread_attr_t*)this->priv->argThreadV;
		delete (pthread_t*)this->priv->pThreadV;

		delete this->priv;
	}

	//------------------------------------------------
}

#else
	#error
#endif

namespace Threading
{
	void Thread::sleepCurrentThread(int miliseconds)
	{
		Utils::sleepMs(miliseconds);
	}
}
