
#include "../include/Utils.h"

#include "../include/MemoryPool.h"

#include <map>
#include <algorithm>
#include <cstring>

//MemoryPool* MemoryPool::instance = NULL;

const bool DEBUG_VERBOSE = false;
const bool ALLOWS_MALLOC_ELSEWHERE = true;

//---------------------------------------------------------------------

void MemoryPool::allocateGlobal(size_t size)
{
	Assert(m_data == NULL);
	m_size = size;
	m_data = malloc(size);
	Assert(m_data != NULL);
}

//---------------------------------------------------------------------

void MemoryPool::unallocateGlobal()
{
	if (m_data != NULL)
	{
		free(m_data);
		m_data = NULL;
	}
}

//---------------------------------------------------------------------

bool MemoryPool::isOkToAllocateHere_(void* ptr, size_t size)
{
	size_t ptrs = (size_t)ptr;
	iterateMapConst(m_chunks, void*, size_t)
	{
		size_t beginOtherChunk = (size_t)((*it).first);
		if (ptrs == beginOtherChunk)
		{
			//outputln("not ok all 1: " << ptrs << "," << beginOtherChunk);
			return false;
		}
		else if (ptrs < beginOtherChunk)
		{
			size_t sizeAvailableHere = beginOtherChunk-ptrs;
			if (sizeAvailableHere < size)
			{
				//outputln("not ok all 2: " << ptrs << "," << size << "," << sizeAvailableHere);
				return false;
			}
		}
		else if (ptrs + size > ((size_t)m_data) + m_size)
		{
			//outputln("not ok all 3: " << ptrs << "," << size << ", " << m_size);
			return false;
		}
	}
	return true;
}

//---------------------------------------------------------------------

void* MemoryPool::allocate(size_t size)
{
	if (DEBUG_VERBOSE) outputln("allocate: " << size << " (nbChunks=" << m_chunks.size() << ", nbAlloc=" << m_nbAlloc << ")");
	Assert(size > 0);
	++m_nbAlloc;
	if (isOkToAllocateHere_(m_data, size))
	{
		m_chunks[m_data] = size;
		return m_data;
	}
	iterateMapConst(m_chunks, void*, size_t)
	{
		size_t startChunkS = (size_t)(*it).first;
		size_t endChunkS = startChunkS+(*it).second;
		void* endChunk = (void*)endChunkS;
		Assert(endChunk > (*it).first);
		if (isOkToAllocateHere_(endChunk, size))
		{
			//if (endChunkS + size > m_size) continue;//break;
			m_chunks[endChunk] = size;
			return endChunk;
		}
		//Assert(false);
	}
	if (ALLOWS_MALLOC_ELSEWHERE)
	{
		if (DEBUG_VERBOSE) outputln("(allocate elsewhere)");
		return malloc(size);
	}

	Assert(false);
	return NULL;
}

//---------------------------------------------------------------------

void MemoryPool::unallocate(void* ptr)
{
	--m_nbAlloc;
	bool foundChunk = (m_chunks.find(ptr) != m_chunks.end());
	if (ALLOWS_MALLOC_ELSEWHERE && !foundChunk)
	{
		if (DEBUG_VERBOSE) outputln("unallocate (elsewhere): " << (size_t)m_data << "(nbChunks=" << m_chunks.size() << ", nbAlloc=" << m_nbAlloc << ")");
		free(ptr);
	}
	else
	{
		if (DEBUG_VERBOSE) outputln("unallocate: " << ((size_t)ptr-(size_t)m_data) << "(sizeChunk=" << m_chunks[ptr]<< ", nbChunks=" << m_chunks.size() << ", nbAlloc=" << m_nbAlloc << ")");
		Assert(foundChunk);
		m_chunks.erase(ptr);
	}
}

//---------------------------------------------------------------------

void* MemoryPool::reallocate(void* ptr, size_t size)
{
	bool foundChunk = (m_chunks.find(ptr) != m_chunks.end());
	Assert(ALLOWS_MALLOC_ELSEWHERE || foundChunk);
	if (foundChunk)
	{
		size_t oldSize = m_chunks[ptr];
		if (size <= oldSize)
		{
			if (DEBUG_VERBOSE) outputln("reallocate (smaller): " << ptr << " with size: " << size << " (nbChunks=" << m_chunks.size() << ", nbAlloc=" << m_nbAlloc << ")");
			m_chunks[ptr] = size;
			return ptr;
		}
		else
		{
			iterateMapConst(m_chunks, void*, size_t)
			{
				void* chunkPtr = (*it).first;
				if (chunkPtr!=ptr)
				{
					if (((size_t)ptr)+size > ((size_t)chunkPtr))
					{
						void* newPtr = allocate(size);
						if (DEBUG_VERBOSE)
							outputln("reallocate (bigger, not enough room): " << ptr << " to " << newPtr << " with size: " << size << " (nbChunks=" << m_chunks.size() << ", nbAlloc=" << m_nbAlloc << ")");
						Assert(newPtr != NULL);
						memcpy(newPtr, ptr, oldSize);
						unallocate(ptr);
						return newPtr;
					}
				}
			}
			if (DEBUG_VERBOSE) outputln("reallocate (bigger, enough room): " << (size_t)m_data << " with size: " << size << " (nbChunks=" << m_chunks.size() << ", nbAlloc=" << m_nbAlloc << ")");
			m_chunks[ptr] = size;
			return ptr;
		}
	}
	else if (ALLOWS_MALLOC_ELSEWHERE)
	{
		if (DEBUG_VERBOSE) outputln("reallocate (still elsewhere): " << (size_t)m_data << " with size: " << size << " (nbChunks=" << m_chunks.size() << ", nbAlloc=" << m_nbAlloc << ")");
		return realloc(ptr, size);
	}
	
	Assert(false);
	return NULL;
}

//---------------------------------------------------------------------

bool MemoryPool::isEmpty()
{
	return m_chunks.size() == 0 && m_nbAlloc == 0;
}

//---------------------------------------------------------------------

void MemoryPool::getInfos(size_t* nbChunks, size_t* sizeAllocated, size_t* nbAlloc) const
{
	*nbChunks = m_chunks.size();
	*nbAlloc = m_nbAlloc;
	*sizeAllocated = 0;
	iterateMapConst(m_chunks, void*, size_t)
	{
		*sizeAllocated += (*it).second;
	}
}

//---------------------------------------------------------------------
