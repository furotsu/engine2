#include "globals.hlsli"

cbuffer cbPerObject : register(b2)
{
	row_major float4x4 g_meshMat;
};

struct VSIn
{
	float3 position : POSITION;
	float4 rowX : ROWX;
	float4 rowY : ROWY;
	float4 rowZ : ROWZ;
	float4 rowW : ROWW;
};

struct VSout
{
	float4 posWS : SV_POSITION;
};

VSout VSMain(VSIn input)
{
	VSout output;
	float4x4 instanceMat = float4x4(input.rowX, input.rowY, input.rowZ, input.rowW);
	float4x4 modelMat = mul(g_meshMat, instanceMat);
	output.posWS = mul(float4(input.position, 1.0f), modelMat);
	return output;
}

//Geometry shader
struct GSout
{
	float4 position : SV_POSITION;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

cbuffer cbPerObject : register(b2)
{
	row_major float4x4 g_viewProjMat[6];
	uint g_textureCubePos;
	float3 paddingGeom;
};

[maxvertexcount(18)]
void GSMain(triangle VSout input[3], inout TriangleStream<GSout> CubeMapStream)
{
	[unroll]
	for (int f = 0; f < 6; ++f)
	{
		GSout output = (GSout)0;

		output.RTIndex = 6 * g_textureCubePos + f;
		
		[unroll]
		for (int v = 0; v < 3; ++v)
		{
			float4 worldPosition = input[v].posWS;
			output.position = mul(worldPosition, g_viewProjMat[f]);
			CubeMapStream.Append(output);
		}
		CubeMapStream.RestartStrip();
	}
}