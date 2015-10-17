#ifndef ConstantBuffer_h_INCLUDED
#define ConstantBuffer_h_INCLUDED

#include "../include/BasicMath.h"

struct ConstantBuffer
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

#endif //ConstantBuffer_h_INCLUDED
