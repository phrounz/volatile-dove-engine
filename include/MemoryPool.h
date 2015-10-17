#ifndef MemoryPool_h_INCLUDED
#define MemoryPool_h_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <map>

//---------------------------------------------------------------------
/**
* \brief A <a href="https://en.wikipedia.org/wiki/Memory_pool">memory pool</a>
*/

class MemoryPool
{
public:
	MemoryPool(size_t size) : m_data(NULL), m_size(-1), m_nbAlloc(0) { this->allocateGlobal(size); }

	void* allocate(size_t size);
	void unallocate(void* ptr);
	void* reallocate(void* ptr, size_t size);
	bool isEmpty();

	inline size_t getTotalSize() const { return m_size; }
	void getInfos(size_t* nbChunks, size_t* sizeAllocated, size_t* nbAlloc) const;

	~MemoryPool() { this->unallocateGlobal(); }
private:
	void allocateGlobal(size_t size);
	void unallocateGlobal();

	bool isOkToAllocateHere_(void* ptr, size_t size);

	//MemoryPool() : m_data(NULL), m_size(-1) {}
	std::map<void*, size_t> m_chunks;
	void* m_data;
	size_t m_size;
	//static MemoryPool* instance;
	size_t m_nbAlloc;
};

//---------------------------------------------------------------------

#endif //MemoryPool_h_INCLUDED
