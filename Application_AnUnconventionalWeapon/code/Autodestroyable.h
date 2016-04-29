#ifndef Autodestroyable_h_INCLUDED
#define Autodestroyable_h_INCLUDED

#include <list>

class Autodestroyable
{
public:
	bool isDestroyed() const { return m_destroyed; }

	template <class T>
	static void cleanupList(std::list<T*>& listToCleanup)
	{
		for(typename std::list<T*>::iterator it = listToCleanup.begin(); it != listToCleanup.end();)
		{
			if ((*it)->isDestroyed())
			{
				delete *it;
				it = listToCleanup.erase(it);
			}
			else
				it++;
		}
	}

protected:
	Autodestroyable():m_destroyed(false) {}
	void autodestroy() { m_destroyed = true; }
private:
	bool m_destroyed;
};



#endif //Autodestroyable_h_INCLUDED
