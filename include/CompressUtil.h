
#ifndef CompressUtil_h_INCLUDED
#define CompressUtil_h_INCLUDED

// NOTE: if USES_WINDOWS8_METRO is defined, it doesn't actually compress

namespace CompressUtil
{
	struct CompressedBuffer
	{
		void* dataCompressed;
		unsigned long lengthCompressed;
		unsigned long lengthUncompressed;
	};
	CompressedBuffer compressData(const void* inData, unsigned long inLength);
	void uncompressDataInto(const CompressedBuffer& cb, void* destBuf);
	void* uncompressData(const CompressedBuffer& cb);
}

#endif //CompressUtil_h_INCLUDED
