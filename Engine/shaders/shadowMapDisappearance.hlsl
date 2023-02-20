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
	float3 spherePos : SPHEREPOS;
	float sphereGrowthSpeed : SPHEREGROWTHSPEED;
};

struct VSout
{
	float4 PosWS : SV_POSITION;
	float startTimePoint : TIMEPOINT;
	float2 texCoord : TEXCOORD;
	float3 spherePos : SPHEREPOS;
	float sphereGrowthSpeed : SPHEREGROWTHSPEED;
};

VSout VSMain(VSIn input)
{
	VSout output;
	float4x4 instanceMat = float4x4(input.rowX, input.rowY, input.rowZ, input.rowW);
	float4x4 modelMat = mul(g_meshMat, instanceMat);
	output.PosWS = mul(float4(input.position, 1.0f), modelMat);
	output.startTimePoint = input.startTimePoint;
	output.texCoord = input.texCoord;
	output.spherePos = input.spherePos;
	output.sphereGrowthSpeed = input.sphereGrowthSpeed;
	return output;
}

//Geometry shader
struct GSOut
{
	float4 position : SV_POSITION;
	float3 PosWS: POSITION;
	float startTimePoint : TIMEPOINT;
	float2 texCoord : TEXCOORD;
	float3 spherePos : SPHEREPOS;
	float sphereGrowthSpeed : SPHEREGROWTHSPEED;
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
			float4 worldPosition = input[v].PosWS;
			output.position = mul(worldPosition, g_viewProjMat[f]);
			output.PosWS = input[v].PosWS;
			output.startTimePoint = input[v].startTimePoint;
			output.texCoord = input[v].texCoord;
			output.spherePos = input[v].spherePos;
			output.sphereGrowthSpeed = input[v].sphereGrowthSpeed;
			CubeMapStream.Append(output);
		}
		CubeMapStream.RestartStrip();
	}
}

Texture2D g_perlinNoise : register(t4);

void PSMain(GSOut input)
{
	float3 lineColor = float3(0.9f, 0.3f, 0.02f);
	float deltaTime = (g_timePoint - input.startTimePoint);

	float noiseValue = g_perlinNoise.Sample(g_sampler, input.texCoord).r;

	float radius = (g_timePoint - input.startTimePoint) * input.sphereGrowthSpeed;
	float lineWidth = 0.1f;
	float dist = length(input.spherePos - input.PosWS);

	if (dist < radius + lineWidth * noiseValue)
	{
		if (!(dist > radius - lineWidth * noiseValue))
		{
			discard;
		}
	}
}