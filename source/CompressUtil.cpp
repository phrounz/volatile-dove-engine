
#if !defined(USES_WINDOWS8_METRO) && !defined(_WIN64) && !defined (__x86_64__) && !defined(__ppc64__)
	#define COMPILE_COMPRESS
#endif

#ifdef COMPILE_COMPRESS
	#include <cstdio>
	#include <cstdlib>
	#include <zlib.h>
#endif

#include "../include/CompressUtil.h"
#include "../include/Utils.h"

namespace CompressUtil
{
//-------------------------------------------------------------------------

CompressedBuffer compressData(const void* inData, unsigned long inLength)
{
	CompressedBuffer cb;
#ifdef COMPILE_COMPRESS
	cb.lengthUncompressed = inLength;
	cb.lengthCompressed = (unsigned long)(inLength*1.1f)+1+12;
	cb.dataCompressed = malloc(cb.lengthCompressed);

	int res = compress((Bytef*)cb.dataCompressed, &cb.lengthCompressed, (Bytef*)inData, inLength);
	Assert(res == Z_OK);

	//outputln("size uncompressed: " << inLength << ", size compressed: " << cb.lengthCompressed);
	cb.dataCompressed = realloc(cb.dataCompressed, cb.lengthCompressed);
	Assert(cb.dataCompressed != NULL);

#else
	cb.lengthUncompressed = inLength;
	cb.lengthCompressed = inLength;
	cb.dataCompressed = malloc(cb.lengthCompressed);
	memcpy(cb.dataCompressed, inData, inLength);
#endif
	return cb;
}

//-------------------------------------------------------------------------

void uncompressDataInto(const CompressedBuffer& cb, void* destBuf)
{
#ifdef COMPILE_COMPRESS
	unsigned long outLength = cb.lengthUncompressed;
	int res = uncompress((Bytef*)destBuf, &outLength, (Bytef*)cb.dataCompressed, cb.lengthCompressed);
	Assert(res == Z_OK);
	Assert(outLength == cb.lengthUncompressed);
	//outputln("size compressed: " << cb.lengthCompressed << ", size uncompressed: " << outLength);
#else
	Assert(cb.lengthCompressed == cb.lengthUncompressed);
	memcpy(cb.dataCompressed, destBuf, cb.lengthCompressed);
#endif
}

//-------------------------------------------------------------------------

void* uncompressData(const CompressedBuffer& cb)
{
	void* outData = malloc(cb.lengthUncompressed);
	uncompressDataInto(cb, outData);
	//outData = realloc(outData, outLength);
	//Assert(outData != NULL);
	return outData;
}

//-------------------------------------------------------------------------
}
