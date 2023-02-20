#include "globals.hlsli"

struct VSOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

VSOut VSMain(uint vertexId : SV_VertexID)
{
	VSOut output;

	float2 vertexCS[3] = { {-1.0f, 3.0f}, {3.0f, -1.0f}, {-1.0f, -1.0f} };
	float2 texCoords[3] = { {0.0f, -1.0f}, {2.0f, 1.0f}, {0.0f, 1.0f} };
	output.position = float4(vertexCS[vertexId], 0.0f, 1.0f);
	output.texCoord = texCoords[vertexId];

	return output;
}

// Pixel shader

Texture2D depthTexture : register(t0);

struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};


float4 PSMain(PSIn input) : SV_TARGET
{
	float depth = depthTexture.Sample(g_sampler, input.texCoord);

	return float4(depth, depth, depth, 1.0f);
}
