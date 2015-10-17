//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//----------------------------------------------------------------------

//#define MULTISAMPLING

#ifdef MULTISAMPLING
	Texture2DMS<float4, 4> simpleTexture : register(t0);
#else
	Texture2D simpleTexture : register(t0);
#endif

SamplerState simpleSampler : register(s0);

//cbuffer TransparentBuffer{float blendAmount;};

cbuffer simplePixelInfoBuffer : register(b0)
{
	int blurLevel;
	int usesLight;
	int tmp1;
	int tmp2;
	float4 fogColor;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD0;
	float fogFactor : FOG;
	float4 color : COLOR0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 textureColor;
	float4 finalColor;

#ifdef MULTISAMPLING

	//textureColor = float4(5.f, 5.f, 5.f, 5.f); 
	textureColor = float4(0.f, 0.f, 0.f, 0.f);

	/*for (int i = 0; i < 8; i++)
	{
		textureColor += simpleTexture.Load(input.tex, 0) * input.color;
		//simpleTexture.SampleCmp(input.tex);
	}*/
	textureColor += simpleTexture.Load(input.tex, 0) * input.color;
	//textureColor = textureColor / 8.f;
	//textureColor = float4(1.f, 1.f, 1.f, 1.f);
	
#else

	// Sample the texture pixel at this location.
	textureColor = simpleTexture.Sample(simpleSampler, input.tex) * input.color;
	
#endif
	
	if (usesLight)
	{
		// In the vertex shader, the normals were transformed into the world space,
		// so the lighting vector here will be relative to the world space.
		float finalAlpha = finalColor.a;
		float3 lightDirection = normalize(float3(-1, -1, -1));// direction of the sun
			float lightMagnitude = 0.5f * saturate(dot(input.norm, -lightDirection)) + 0.5f;
		float4 finalColorWithLight = textureColor * lightMagnitude;
			finalColorWithLight.a = finalAlpha;
		finalColor = finalColorWithLight;
	}
	else
	{
		finalColor = textureColor;
	}

	// Calculate the final color using the fog effect equation.
	return finalColor * input.fogFactor + fogColor * (1.0 - input.fogFactor);
}
