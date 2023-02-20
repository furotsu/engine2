#include "globals.hlsli"

cbuffer cbPerObject : register(b2)
{
	row_major float4x4 g_meshMat;
};

struct VSIn
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
	float4 rowX : ROWX;
	float4 rowY : ROWY;
	float4 rowZ : ROWZ;
	float4 rowW : ROWW;
	float startTimePoint : TIMEPOINT;
};

struct VSout
{
	float4 posWS : SV_POSITION;
	float startTimePoint : TIMEPOINT;
	float2 texCoord : TEXCOORD;
};

VSout VSMain(VSIn input)
{
	VSout output;
	float4x4 instanceMat = float4x4(input.rowX, input.rowY, input.rowZ, input.rowW);
	float4x4 modelMat = mul(g_meshMat, instanceMat);
	output.posWS = mul(float4(input.position, 1.0f), modelMat);
	output.startTimePoint = input.startTimePoint;
	output.texCoord = input.texCoord;
	return output;
}

//Geometry shader
struct GSOut
{
	float4 position : SV_POSITION;
	float startTimePoint : TIMEPOINT;
	float2 texCoord : TEXCOORD;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

cbuffer cbPerObject : register(b2)
{
	row_major float4x4 g_viewProjMat[6];
	uint g_textureCubePos;
	float3 paddingGeom;
};

[maxvertexcount(18)]
void GSMain(triangle VSout input[3], inout TriangleStream<GSOut> CubeMapStream)
{
	[unroll]
	for (int f = 0; f < 6; ++f)
	{
		GSOut output = (GSOut)0;

		output.RTIndex = 6 * g_textureCubePos + f;

		[unroll]
		for (int v = 0; v < 3; ++v)
		{
			float4 worldPosition = input[v].posWS;
			output.position = mul(worldPosition, g_viewProjMat[f]);
			output.startTimePoint = input[v].startTimePoint;
			output.texCoord = input[v].texCoord;
			CubeMapStream.Append(output);
		}
		CubeMapStream.RestartStrip();
	}
}

Texture2D g_perlinNoise : register(t4);

void PSMain(GSOut input)
{
	float animationTime = 5.0f;
	
	float3 lineColor = float3(0.9f, 0.3f, 0.02f);
	float lineWidth = 0.01f;
	float deltaTime = (g_timePoint - input.startTimePoint);
	
	float noiseValue = g_perlinNoise.Sample(g_sampler, input.texCoord).r;
	float noiseCompValue = deltaTime / g_dissolutionSpawnTime;
	
	if (noiseCompValue < noiseValue)
	{
		if (!(noiseCompValue > noiseValue - lineWidth))
			discard;
	}
}