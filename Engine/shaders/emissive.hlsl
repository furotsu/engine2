#include "globals.hlsli"
#include "pbrRes.hlsli"

// Vertex shader
cbuffer cbPerObject : register(b2)
{
	row_major float4x4 g_meshMat;
};

struct VSIn
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 rowX : ROWX;
	float4 rowY : ROWY;
	float4 rowZ : ROWZ;
	float4 rowW : ROWW;
};

struct VSOut
{
	float4 projPos : SV_POSITION;
	float3 normal : NORMAL;
	float3 PosWS : POSITION;
	uint instanceId : INSTANCE_ID;
};

VSOut VSMain(VSIn input, uint instanceId : SV_InstanceID)
{
	VSOut output;

	float4x4 instanceMat = float4x4(input.rowX, input.rowY, input.rowZ, input.rowW);
	float4x4 modelMat = mul(g_meshMat, instanceMat);

	output.PosWS = mul(float4(input.position, 1.0f), modelMat).xyz;
	output.projPos = mul(float4(output.PosWS, 1.0f), g_viewProj);

	output.normal = input.normal;
	output.instanceId = instanceId;

	return output;
}
//Pixel shader

cbuffer cbPerObject : register(b2)
{
	uint lightPos;
	float3 padding2;
};

struct PSOut
{
	float2 normalPacked : SV_Target0;
	float4 albedo: SV_Target1;
	float2 roughMetal: SV_Target2;
	float4 emission: SV_Target3;
	uint objectID: SV_Target4;
};

PSOut PSMain(VSOut input) : SV_TARGET
{
	PSOut output; 

	float3 emission = g_pointLight[lightPos].color * g_pointLight[lightPos].radiance;
	
	float3 cameraDir = normalize(g_cameraPos - input.PosWS);
	
	float3 normedEmission = emission / max(emission.x, max(emission.y, max(emission.z, 1.0)));
	
	float NoV = dot(cameraDir, input.normal);

	output.normalPacked = packOctahedron(input.normal);
	output.albedo = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.roughMetal = float2(0.999f, 0.001f);
	output.emission = float4(lerp(normedEmission * 0.33, emission, pow(max(0.0, NoV), 8)), 1.0f);
	output.objectID = -1;

	return output;

}
