#ifndef BufferUtils_h_INCLUDED
#define BufferUtils_h_INCLUDED

#include <vector>
#include <set>
#include <cstdio>
#include <cstdlib>

#include "MemoryPool.h"
#include "CoreUtils.h"
#include "Utils.h"
#include "CompressUtil.h"

#define AssertTDB Assert

namespace BufferUtils
{
	//---------------------------------------------------------------------
	/**
	* \brief Buffer which can accept mixed types data
	*/

	class BufferOfAnyType
	{
	public:
		/// Add an int at the end of the buffer
		void addInt(i32 value)
		{
			buffer.resize(buffer.size() + sizeof(i32));
			memcpy(&buffer[buffer.size() - sizeof(i32)], &value, sizeof(i32));
		}
		/// Add a Float3 the end of the buffer
		void addFloat3(const Float3& value)
		{
			buffer.resize(buffer.size() + sizeof(Float3));
			memcpy(&buffer[buffer.size() - sizeof(Float3)], &value, sizeof(Float3));
		}
		/// Add a thing at the end of the buffer, which is of size size in bytes.
		void add(const void* thing, size_t size)
		{
			buffer.resize(buffer.size() + size);
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
			memcpy(&(buffer.data()[buffer.size() - size]), thing, size);
#else
			memcpy(&(buffer[buffer.size() - size]), thing, size);
#endif
		}
		void* getData() { return (void*)(&buffer[0]); }
		int getSize() { return (int)buffer.size(); }
	private:
		std::vector<unsigned char> buffer;
	};

	//---------------------------------------------------------------------
	/**
	* \brief Buffer optimized to be often filled with new data
	*/
	
	template<class T>
	class ResizeableBuffer
	{
	public:
		ResizeableBuffer(MemoryPool* memoryPoolOrNull) :m_buf(NULL), m_size(0), m_realSize(0), m_memoryPoolOrNull(memoryPoolOrNull)
		{
		}
		ResizeableBuffer& operator=(const ResizeableBuffer& res)
		{
			this->resize(res.size());
			memcpy(m_buf, res.m_buf, res.size()*sizeof(T));
			return *this;
		}

		void mergeWith(const ResizeableBuffer& anotherResizeableBuffer)
		{
			size_t sizePrev = m_size;
			if (m_size + anotherResizeableBuffer.m_size <= m_realSize)
				m_size += anotherResizeableBuffer.m_size;
			else
				this->resize(m_size + anotherResizeableBuffer.m_size);
			memcpy(this->getPtr(sizePrev), anotherResizeableBuffer.getPtr(0), anotherResizeableBuffer.m_size*sizeof(T));
		}

		void resize(size_t newSize)
		{
			//if (m_memoryPoolOrNull != NULL)
			//	Assert(newSize < m_memoryPoolOrNull->getTotalSize() / sizeof(T));

			if (newSize != m_size)
			{
				m_size = newSize;
				size_t sizeGap = (m_size > 1000000 ? 2621440 : (m_size > 100000 ? 262144 : (m_size > 1000 ? 32768 : 256)));
				size_t newRealSize = m_size == 0 ? 0 : m_size - m_size % sizeGap + sizeGap;
				Assert(newRealSize >= m_size);
				if (newRealSize != m_realSize)
				{
					//outputln("ResizeableBuffer: Realloc from " << m_realSize << " to " << newRealSize);
					m_realSize = newRealSize;
					if (m_size == 0)
					{
						if (m_buf != NULL)
						{
							if (m_memoryPoolOrNull != NULL)
								m_memoryPoolOrNull->unallocate(m_buf);
							else
								free(m_buf);

							m_buf = NULL;
						}
					}
					else
					{
						if (m_buf == NULL)
						{
							m_buf = (T*)(m_memoryPoolOrNull != NULL ? m_memoryPoolOrNull->allocate(sizeof(T)*m_realSize) : malloc(sizeof(T)*m_realSize));
						}
						else
						{
							m_buf = (T*)(m_memoryPoolOrNull != NULL ? m_memoryPoolOrNull->reallocate(m_buf, sizeof(T)*m_realSize) : realloc(m_buf, sizeof(T)*m_realSize));
						}
						Assert(m_buf != NULL);
					}
				}
			}
		}
		inline size_t size() const   { return m_size; }
		inline T* data()             { return m_buf; }
		inline const T* data() const { return m_buf; }
		inline T& get(size_t pos)    { return m_buf[pos]; }
		inline T* getPtr(size_t pos) { return &m_buf[pos]; }
		inline const T* getPtr(size_t pos) const { return &m_buf[pos]; }
		/*void add(const T& v)
		{
		this->resize(size+1);
		memcpy(m_buf[size-1], &v, sizeof(T));
		}*/
		~ResizeableBuffer()
		{
			this->resize(0);
		}
	private:
		T* m_buf;
		size_t m_size;// user required size
		size_t m_realSize;// allocation size of m_buf (shall be >= m_size)
		MemoryPool* m_memoryPoolOrNull;
	};

	//---------------------------------------------------------------------
	/**
	* \brief 2 dimensions buffer
	*/

	template<class T>
	class TwodimensionalBuffer
	{
	public:
		TwodimensionalBuffer(const Int2& size) :m_size(size), m_buffer(NULL)
		{
			m_buffer = new T[size.x()*size.y()];
		}
		inline void setValueFromCopy(const Int2& pos, T value)
		{
			AssertTDB(pos >= Int2(0, 0) && pos < m_size);
			m_buffer[pos.y() * m_size.x() + pos.x()] = value;
		}
		inline T& getValueAsRef(const Int2& pos)
		{
			AssertTDB(pos >= Int2(0, 0) && pos < m_size);
			return m_buffer[pos.y() * m_size.x() + pos.x()];
		}
		bool isIn(const Int2& pos) const
		{
			return pos >= Int2(0, 0) && pos < m_size;
		}
		inline const T& getValueAsConstRef(const Int2& pos) const
		{
			AssertTDB(pos >= Int2(0, 0) && pos < m_size);
			return m_buffer[pos.y() * m_size.x() + pos.x()];
		}
		inline const Int2& getSize() const { return m_size; }
		inline const Int2& size() const { return m_size; }
		~TwodimensionalBuffer()
		{
			delete m_buffer;
		}
	private:
		Int2 m_size;
		T* m_buffer;
	};

	//---------------------------------------------------------------------
	/**
	* \brief 3 dimensions buffer
	*/

	template<class T>
	class TridimensionalBuffer
	{
	public:
		TridimensionalBuffer(const Int3& size) :m_size(size), m_buffer(NULL)
		{
			m_buffer = new T[size.x()*size.y()*size.z()];
		}
		TridimensionalBuffer(void* alreadyAllocatedBuffer, const Int3& size)
		{
			m_size = size;
			m_buffer = (T*)alreadyAllocatedBuffer;
		}
		TridimensionalBuffer(const TridimensionalBuffer& otherInstance) : m_buffer(NULL)
		{
			*this = otherInstance;
		}
		TridimensionalBuffer& operator=(const TridimensionalBuffer& otherInstance)
		{
			this->copyFrom(otherInstance);
			return *this;
		}

		void copyFrom(const TridimensionalBuffer& otherInstance)
		{
			if (m_buffer != NULL) delete[] m_buffer;

			m_size = otherInstance.m_size;
			size_t sizeBuf = m_size.x()*m_size.y()*m_size.z()*sizeof(T);
			m_buffer = new T[sizeBuf];
			memcpy(m_buffer, otherInstance.m_buffer, sizeBuf);
		}

		//--------------------
		// applies on a point (x,y,z)

		inline void setValueFromCopy(const Int3& pos, T value)
		{
			AssertTDB(pos >= Int3(0, 0, 0) && pos < m_size);
			//AssertTDB(pos.z()*m_size.y()*m_size.x() + pos.y() * m_size.x() + pos.x() >= 0);
			//AssertTDB(pos.z()*m_size.y()*m_size.x() + pos.y() * m_size.x() + pos.x() < m_size.x()*m_size.y()*m_size.z());
			m_buffer[pos.z()*m_size.y()*m_size.x() + pos.y() * m_size.x() + pos.x()] = value;
		}
		inline void setValueFromCopy(int x, int y, int z, T value)
		{
			m_buffer[z*m_size.y()*m_size.x() + y * m_size.x() + x] = value;
		}

		inline T getValueAsCopy(const Int3& pos) const
		{
			AssertTDB(pos >= Int3(0, 0, 0) && pos < m_size);
			return m_buffer[pos.z()*m_size.y()*m_size.x() + pos.y() * m_size.x() + pos.x()];
		}
		inline T& getValueAsRef(const Int3& pos)
		{
			AssertTDB(pos >= Int3(0, 0, 0) && pos < m_size);
			return m_buffer[pos.z()*m_size.y()*m_size.x() + pos.y() * m_size.x() + pos.x()];
		}
		inline const T& getValueAsConstRef(const Int3& pos) const
		{
			AssertTDB(pos >= Int3(0, 0, 0) && pos < m_size);
			return m_buffer[pos.z()*m_size.y()*m_size.x() + pos.y() * m_size.x() + pos.x()];
		}
		inline T getValueAsCopy(int x, int y, int z) const
		{
			return m_buffer[z*m_size.y()*m_size.x() + y * m_size.x() + x];
		}

		inline bool isPosInRange(const Int3& pos) const { return pos >= Int3(0, 0, 0) && pos < m_size; }

		//--------------------
		// applies on a segment from (pos.x,pos.y,pos.z) to (pos.x+sizeX,pos.y,pos.z)

		inline void setValueSegmentX(const Int3& pos, size_t sizeX, T value)
		{
			Assert(sizeof(T) == 1);
			memset(&m_buffer[pos.z()*m_size.y()*m_size.x() + pos.y() * m_size.x() + pos.x()], value, sizeX);
		}

		//--------------------
		// applies on a cuboid from (begin.x,begin.y,begin.z) to (end.x,end.y,end.z) included

		inline void setValueRange(const Int3& begin, const Int3& end, T value)
		{
			for (int z = begin.z(); z <= end.z(); ++z)
				for (int y = begin.y(); y <= end.y(); ++y)
					//this->setValueSegmentX(Int3(begin.x(),y,z), end.x()-begin.x()+1, value);
					for (int x = begin.x(); x <= end.x(); ++x)
						this->setValueFromCopy(x, y, z, value);
		}
		inline std::set<T> getAllValuesInRange(const Int3& begin, const Int3& end) const
		{
			std::set<T> vals;
			T lastValForOptim = getValueAsCopy(begin);//getValue
			vals.insert(lastValForOptim);
			for (int x = begin.x(); x <= end.x(); ++x)
				for (int y = begin.y(); y <= end.y(); ++y)
					for (int z = begin.z(); z <= end.z(); ++z)
						if (lastValForOptim != getValueAsCopy(x, y, z))//getValue
							vals.insert(getValueAsCopy(x, y, z));//getValue
			return vals;
		}
		inline bool areAllValuesInRangeEqualsTo(const Int3& begin, const Int3& end, const T& value) const
		{
			for (int x = begin.x(); x <= end.x(); ++x)
				for (int y = begin.y(); y <= end.y(); ++y)
					for (int z = begin.z(); z <= end.z(); ++z)
						if (this->getValueAsCopy(x, y, z) != value)
							return false;
			return true;
		}
		inline bool areAllValuesInRangeNotEqualsTo(const Int3& begin, const Int3& end, const T& value) const
		{
			for (int x = begin.x(); x <= end.x(); ++x)
				for (int y = begin.y(); y <= end.y(); ++y)
					for (int z = begin.z(); z <= end.z(); ++z)
						if (this->getValueAsCopy(x, y, z) == value)
							return false;
			return true;
		}

		//--------------------
		// applies everywhere

		void setAllValues(T value)
		{
			if (sizeof(T) == sizeof(unsigned char) || value == 0)
				memset(m_buffer, (int)value, m_size.x()*m_size.y()*m_size.z()*sizeof(T));
			else
				for (int i = 0; i < m_size.x()*m_size.y()*m_size.z(); ++i) m_buffer[i] = value;
		}

		//--------------------
		// misc

		virtual ~TridimensionalBuffer()
		{
			delete[] m_buffer;
		}
		inline const Int3& getSize() const { return m_size; }

		T* getBuffer() { return m_buffer; }
		const T* getBuffer() const { return m_buffer; }

		CompressUtil::CompressedBuffer createCompressedBuffer() const
		{
			CompressUtil::CompressedBuffer cb = CompressUtil::compressData(m_buffer, m_size.x()*m_size.y()*m_size.z()*sizeof(T));
			return cb;
		}

		void clearAndResize(const Int3& newSize)
		{
			delete[] m_buffer;
			m_size = newSize;
			m_buffer = new T[m_size.x()*m_size.y()*m_size.z()];
		}

	private:
		Int3 m_size;
		T* m_buffer;
	};

	//---------------------------------------------------------------------
	/**
	* \brief 3 dimensions buffer, which indexes can be negative
	*/

	template<class T>
	class ShiftedTridimensionalBuffer : private TridimensionalBuffer<T>
	{
	public:
		ShiftedTridimensionalBuffer(const Int3& size, const Int3& minimum)
			: TridimensionalBuffer<T>(size), m_minimum(minimum) {}

		ShiftedTridimensionalBuffer(ShiftedTridimensionalBuffer& otherInstance)
			: TridimensionalBuffer<T>(otherInstance), m_minimum(otherInstance.m_minimum) {}

		ShiftedTridimensionalBuffer& operator=(const ShiftedTridimensionalBuffer& otherInstance)
		{
			this->copyFrom(otherInstance);
			m_minimum = otherInstance.m_minimum;
			return *this;
		}

		virtual ~ShiftedTridimensionalBuffer() {}

		inline void setValueUnshifted(const Int3& pos, T value) { TridimensionalBuffer<T>::setValueFromCopy(pos, value); }
		inline const T& getValueUnshifted(const Int3& pos) const { return TridimensionalBuffer<T>::getValueAsConstRef(pos); }
		inline bool isInUnshifted(const Int3& pos)  const { return pos >= Int3(0, 0, 0) && pos < this->size(); }

		inline void setValue(const Int3& pos, T value) { TridimensionalBuffer<T>::setValueFromCopy(pos - m_minimum, value); }
		inline const T& getValue(const Int3& pos) const { return TridimensionalBuffer<T>::getValueAsConstRef(pos - m_minimum); }
		inline bool isIn(const Int3& pos)  const { return pos >= m_minimum && pos < m_minimum + this->size(); }

		inline const Int3& size()    const { return TridimensionalBuffer<T>::getSize(); }
		inline const Int3& minimum() const { return m_minimum; }
		inline Int3 maximumPlusOne() const { return m_minimum + TridimensionalBuffer<T>::getSize(); }

		class ConstIterator
		{
			friend class ShiftedTridimensionalBuffer<T>;
		public:
			const ConstIterator& operator++()
			{
				Assert(!m_reachedEnd);
				m_pos.data[2]++;
				if (m_pos.data[2] >= m_father->size().z()) { m_pos.data[2] = 0; m_pos.data[1]++; }
				if (m_pos.data[1] >= m_father->size().y()) { m_pos.data[1] = 0; m_pos.data[0]++; }
				if (m_pos.data[0] >= m_father->size().x()) { m_pos = Int3(0, 0, 0); m_reachedEnd = true; }
				return *this;
			}
			const ConstIterator& operator++(int) { return operator++(); }
			const Int3& posUnshifted() const { return m_pos; }
			Int3 pos() const { return m_pos + m_father->minimum(); }
			const T& value() const { return m_father->getValueUnshifted(m_pos); }
			bool operator!=(const ConstIterator& b) { return m_pos != b.m_pos || m_reachedEnd != b.m_reachedEnd || m_father != b.m_father; }
		private:
			ConstIterator(const ShiftedTridimensionalBuffer<T>* father, bool isAtEnd) : m_father(father), m_pos(0, 0, 0), m_reachedEnd(isAtEnd) {}
			Int3 m_pos;
			const ShiftedTridimensionalBuffer<T>* m_father;
			bool m_reachedEnd;
		};

		ConstIterator begin() const { return ConstIterator(this, false); }
		ConstIterator end()   const { return ConstIterator(this, true); }
		ConstIterator find(const Int3& pos) const
		{
			if (!isIn(pos))
				return end();
			//ConstIterator(this, false) it;
			ConstIterator it(this, false);
			it.m_pos = pos;
			return it;
		}

		void clearAndResize(const Int3& newSize, const Int3& newMinimum) // note: this invalidates all iterators
		{
			TridimensionalBuffer<T>::clearAndResize(newSize);
			m_minimum = newMinimum;
		}
		void setAllValues(T value)
		{
			TridimensionalBuffer<T>::setAllValues(value);
		}

	private:
		Int3 m_minimum;
	};

	//---------------------------------------------------------------------
}

#endif //#ifndef BufferUtils_h_INCLUDED
