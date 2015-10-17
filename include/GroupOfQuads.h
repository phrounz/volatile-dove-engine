#ifndef GroupOfQuads_h_INCLUDED
#define GroupOfQuads_h_INCLUDED

//#define USE_STDVECTOR
#ifdef USE_STDVECTOR
	#include <vector>
#endif
#include "BufferUtils.h"

#include "BasicMath.h"
#include "CoreUtils.h"
#include "CompressUtil.h"
#include "BasicVertex.h"

//---------------------------------------------------------------------

namespace Meshes3D
{
	/**
	* \brief Buffer of BasicQuad, optimized for massive addition of new quads
	*/

	class GroupOfQuads
	{
		friend class GroupOfVBO;
	public:
		GroupOfQuads(MemoryPool* memoryPoolOrNull = NULL) : m_nbQuads(0),
#ifdef USE_STDVECTOR
			m_buf(NULL) { Assert(memoryPoolOrNull == NULL); }
#else
			m_buf(memoryPoolOrNull) {}
#endif
		void mergeWith(const GroupOfQuads& anotherGroupOfQuads)
		{
#ifdef USE_STDVECTOR
			m_buf.resize(m_nbQuads + anotherGroupOfQuads.m_nbQuads);
			memcpy(this->getData(m_nbQuads), &anotherGroupOfQuads.m_buf[0], anotherGroupOfQuads.m_nbQuads*sizeof(BasicQuad));
			m_nbQuads += anotherGroupOfQuads.m_nbQuads;
			//m_buf.insert( m_buf.end(), anotherGroupOfQuads.m_buf.begin(), anotherGroupOfQuads.m_buf.end() );
#else
			m_nbQuads += anotherGroupOfQuads.m_nbQuads;
			m_buf.mergeWith(anotherGroupOfQuads.m_buf);
#endif
		}
		GroupOfQuads(const CompressUtil::CompressedBuffer& cb) : m_buf(NULL)
		{
			m_nbQuads = cb.lengthUncompressed / sizeof(BasicQuad);
			m_buf.resize(m_nbQuads);
			CompressUtil::uncompressDataInto(cb, this->getData());
		}
		CompressUtil::CompressedBuffer getCompressedBuffer()
		{
			return CompressUtil::compressData(this->getData(), (unsigned long)(this->getNbQuads()*sizeof(BasicQuad)));
		}
		void replaceBy(const GroupOfQuads& otherGroupOfQuads)
		{
			m_nbQuads = otherGroupOfQuads.m_nbQuads;
			m_buf = otherGroupOfQuads.m_buf;
		}
		inline size_t getNbQuads() const { return m_nbQuads; }
		inline void setNbQuads(size_t nbQuads) { m_nbQuads = nbQuads; }
		void allocateMoreQuads(size_t parNbQuads)
		{
			if (parNbQuads == 0) return;
			m_buf.resize(m_buf.size() + parNbQuads);
		}

		void addQuads(const float* vertices, const float* normals, const float* texCoord, const float* color, size_t nbNewQuads)
		{
			for (size_t i = 0; i < nbNewQuads; ++i)
				this->getLastAndNext()->copyFrom(&vertices[i * 3 * 4], &normals[i * 3 * 4], &texCoord[i * 2 * 4], &color[i * 3 * 4]);
		}

#ifdef USE_STDVECTOR
		inline const BasicQuad* getData(int index = 0) const { return &m_buf[index]; }
		inline BasicQuad* getData(int index = 0) { return &m_buf[index]; }
		inline BasicQuad* getLast() { Assert(m_buf.size()>=m_nbQuads+1);return &m_buf[m_nbQuads]; }
#else
		inline const BasicQuad* getData(size_t index = 0) const { return &m_buf.data()[index]; }
		inline BasicQuad* getData(size_t index = 0) { return &m_buf.data()[index]; }
		inline BasicQuad* getLast() { Assert(m_buf.size() >= m_nbQuads + 1); return m_buf.getPtr(m_nbQuads); }
#endif
		inline void next() { m_nbQuads++; }

		// polygon reduction
		void mergeTwoPreviousQuadsIfPossible(int faceId, bool isUsualSurface);
		void simplify(bool horiz);

	private:
		inline BasicQuad* getLastAndNext() { BasicQuad* bq = this->getLast(); this->next(); return bq; }
#ifdef USE_STDVECTOR
		std::vector<BasicQuad> m_buf;
#else
		BufferUtils::ResizeableBuffer<BasicQuad> m_buf;
#endif
		size_t m_nbQuads;
	};

	//---------------------------------------------------------------------
}

#endif //GroupOfQuads_h_INCLUDED
