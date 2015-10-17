
#include <cstdio>

#include "../include/Thread.h"
#include "../include/Utils.h"

#include "../include/ParallelTasks.h"

namespace Threading
{
	//---------------------------------------------------------------------

	namespace
	{
		void* runThread(void* paramThreadVoidCasted)
		{
			ParamThreadInterface* paramThread = reinterpret_cast<ParamThreadInterface*>(paramThreadVoidCasted);
			paramThread->run();
			return NULL;
		}
	}

	//---------------------------------------------------------------------

	ParallelTasks::ParallelTasks(std::vector<ParamThreadInterface*> paramThreads)
	{
		m_paramThreads = paramThreads;
		iterateVectorConst(paramThreads, ParamThreadInterface*)
		{
			Thread* thread = new Thread(runThread, (void*)(*it));
			m_threads.push_back(thread);
		}
	}

	//---------------------------------------------------------------------

	void ParallelTasks::waitForTermination()
	{
		/*while (!this->isFinished())
		{
		Thread::sleepCurrentThread(1);
		}*/
		for (size_t i = 0; i < m_threads.size(); ++i)
			delete m_threads[i];// the destructor hangs out until the thread is finished
	}

	//---------------------------------------------------------------------

	bool ParallelTasks::isFinished() const
	{
		iterateVectorConst(m_paramThreads, ParamThreadInterface*)
		{
			if (!(*it)->isFinished()) return false;
		}
		return true;
	}

	//---------------------------------------------------------------------

	ParallelTasks::~ParallelTasks()
	{
		//for (size_t i = 0; i < m_threads.size(); ++i)
		//	delete m_threads[i];
	}

	//---------------------------------------------------------------------
}
