
#ifndef Thread_h_INCLUDED
#define Thread_h_INCLUDED

namespace Threading
{
	class ThreadPriv;

	//---------------------------------------------------------------------
	/**
	* \brief A simple <a href="https://en.wikipedia.org/wiki/Thread_%28computing%29">thread</a>
	*/

	class Thread
	{
	public:
		static void createThread(void*(*func)(void *), void* param);	///< TODO memory leak. Use constructor instead.

		Thread(void*(*func)(void*), void* param);						///< Constructor, creates a thread which starts at once.

		static void sleepCurrentThread(int miliseconds);

		static void initSemaphore();
		static void claimSemaphore();
		static void releaseSemaphore();
		static void deinitSemaphore();

		~Thread();														///< The destructor hangs out until the thread is joined(=finished).
	private:
		ThreadPriv* priv;
	};

	//---------------------------------------------------------------------
}

#endif //Thread_h_INCLUDED
