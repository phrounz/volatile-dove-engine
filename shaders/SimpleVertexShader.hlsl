// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//----------------------------------------------------------------------

cbuffer simpleConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float tmp;
};

cbuffer simpleVertexInfoBuffer : register(b1)
{
	float dataX;
	float dataY;
	float dataZ;
	int userDefinedTextureId;
	float fogStart;
	float fogEnd;
	float windowAspectRatio;
	int foo1;
};

struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD0;
	float4 color : COLOR0;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD0;
	float fogFactor : FOG;
	float4 color : COLOR0;
};
float4x4 projectionMatrix;
float4x4 viewMatrix;
float4x4 modelMatrix;

PixelShaderInput main(VertexShaderInput input)
{
    // Calculate the camera position.
	//cameraPosition = mul(input.pos, world);
	float4 posCopy = float4(input.pos, 1.0f);
	float4 cameraPosition = mul(posCopy, world);
    cameraPosition = mul(cameraPosition, view);
    cameraPosition = mul(cameraPosition, projection);
  
    PixelShaderInput vertexShaderOutput;
    
    // Transform the vertex position into projection space.
	float4 worldPos = mul(float4(input.pos, 1.0f), world);
	float4 viewPos = mul(worldPos, view);
	vertexShaderOutput.pos = mul(viewPos, projection);

    // Pass through the texture coordinate without modification.
    vertexShaderOutput.tex = input.tex;

    // Transform the normal into world space to allow world-space lighting.
    float4 norm = float4(normalize(input.norm), 0.0f);
    norm = mul(norm, world);
    vertexShaderOutput.norm = normalize(norm.xyz);//cross(pos.x, pos.y)

	float distCameraFog = sqrt((cameraPosition.x * windowAspectRatio)*(cameraPosition.x * windowAspectRatio) + cameraPosition.y*cameraPosition.y + cameraPosition.z*cameraPosition.z);
	// Calculate linear fog.
    vertexShaderOutput.fogFactor = saturate((fogEnd - distCameraFog) / (fogEnd - fogStart));

	vertexShaderOutput.color = input.color;

	/*if (userDefinedTextureId > 0)
	{
		if (userDefinedTextureId == 1)
		{
			float timeDur = dataX;
			float4 displacedVertex;
			displacedVertex.x = worldPos.x + 3.0f * cos(worldPos.x / 100.0f + timeDur / 1000.0f) + 3 * sin(worldPos.y / 100.0f + timeDur / 1000.0f);
			displacedVertex.y = worldPos.y + 3.0f * cos(worldPos.y / 100.0f + timeDur / 1000.0f) + 3 * sin(worldPos.z / 100.0f + timeDur / 1000.0f);
			displacedVertex.z = worldPos.z + 3.0f * cos(worldPos.z / 100.0f + timeDur / 1000.0f) + 3 * sin(worldPos.x / 100.0f + timeDur / 1000.0f);
			displacedVertex.w = 1.0f;
			viewPos = mul(displacedVertex, view);
			vertexShaderOutput.pos = mul(viewPos, projection);
		}
		else if (userDefinedTextureId == 2)
		{
			float timeDur = dataX;
			float2 cPos = -1.0 + 2.0 * vertexShaderOutput.tex.xy / float2(10.f, 10.f);
			float cLength = length(cPos);
			vertexShaderOutput.tex.xy = vertexShaderOutput.tex.xy + (cPos / cLength)*cos(cLength*12.0 - timeDur*0.005f)*0.03f;
			vertexShaderOutput.pos.y = vertexShaderOutput.pos.y + 1.0f*cos(worldPos.x * 1000.f + timeDur / 100.f) + 1.0*sin(worldPos.z * 1000.f + timeDur / 100.f);
		}
		else if (userDefinedTextureId == 3)
		{
			float timeDur = dataX;
			float4 displacedVertex;
			displacedVertex.x = worldPos.x + 0.3f * sin(worldPos.y / 100.0f + timeDur / 1000.0f);
			displacedVertex.y = worldPos.y + 0.3f * sin(worldPos.z / 100.0f + timeDur / 1000.0f);
			displacedVertex.z = worldPos.z + 0.3f * sin(worldPos.x / 100.0f + timeDur / 1000.0f);
			displacedVertex.w = 1.0f;
			viewPos = mul(displacedVertex, view);
			vertexShaderOutput.pos = mul(viewPos, projection);
		}
	}*/
	
	return vertexShaderOutput;
}
