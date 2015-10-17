#ifndef ParallelTasks_h_INCLUDED
#define ParallelTasks_h_INCLUDED

#include <vector>

class Thread;

namespace Threading
{
	//---------------------------------------------------------------------
	/**
	* \brief Class for configuring ParallelTasks constructor. 
	* You have to make a class inheriting from this class, instanciate elements, push them on a std::vector, and feed ParallelTasks::ParallelTasks with it.
	*/

	class ParamThreadInterface
	{
	public:
		ParamThreadInterface() : m_isFinished(false) {}
		bool isFinished() const { return m_isFinished; }
		virtual void run() {}
		virtual ~ParamThreadInterface() {}
	protected:
		void finish() { m_isFinished = true; }
	private:
		bool m_isFinished;
	};

	//---------------------------------------------------------------------
	/**
	* \brief Simple multithreading utility which manages parallel tasks.
	*/

	class ParallelTasks
	{
	public:
		ParallelTasks(std::vector<ParamThreadInterface*> paramThreads);///< tasks start at once
		void waitForTermination();///< hangs out until all the tasks are finished
		~ParallelTasks();
	private:
		bool isFinished() const;
		std::vector<ParamThreadInterface*> m_paramThreads;
		std::vector<Thread*> m_threads;
	};

	//---------------------------------------------------------------------
}

#endif //ParallelTasks_h_INCLUDED