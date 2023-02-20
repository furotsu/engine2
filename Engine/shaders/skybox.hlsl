#include "globals.hlsli"

cbuffer cbPerObject : register(b2)
{
    float4 frustum_corners[3];
    float4 camera_position;
};

struct VSOut
{
    float4 position : SV_POSITION;
    float3 dirWS : DIRECTIONWS;
};

VSOut VSMain(uint vertexId : SV_VertexID)
{
    VSOut output;

    float2 vertexCS[3] = { {-1.0f, 3.0f}, {3.0f, -1.0f}, {-1.0f, -1.0f} };
    output.position = float4(vertexCS[vertexId], 0.0f, 1.0f);

    output.dirWS = (frustum_corners[vertexId] - camera_position);
    return output;
}

// Pixel shader

TextureCube objTexture;

float4 PSMain(float4 position : SV_POSITION, float3 dirWS : DIRECTIONWS) : SV_TARGET
{
    return objTexture.Sample(g_sampler, dirWS);
}