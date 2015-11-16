
#ifndef CompressUtil_h_INCLUDED
#define CompressUtil_h_INCLUDED

// NOTE: if USES_WINDOWS8_METRO is defined, it doesn't actually compress
#include <cstdlib>

namespace CompressUtil
{
	struct CompressedBuffer
	{
		void* dataCompressed;
		unsigned long lengthCompressed;
		unsigned long lengthUncompressed;
		void dropData() { free(this->dataCompressed); }
	};
	CompressedBuffer compressData(const void* inData, unsigned long inLength);
	void uncompressDataInto(const CompressedBuffer& cb, void* destBuf);
	void* uncompressData(const CompressedBuffer& cb);
}

#endif //CompressUtil_h_INCLUDED
