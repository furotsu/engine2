#include "pbrRes.hlsli"

cbuffer cbPerObject : register(b2)
{
	row_major float4x4 g_meshMat;
};

struct VSIn
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 rowX : ROWX;
	float4 rowY : ROWY;
	float4 rowZ : ROWZ;
	float4 rowW : ROWW;
	float texCoordScale : TSCALE;
	uint  instanceID : INSTANCEID;
};

struct VSOut
{
	float4 projPos : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 PosWS  : POSITION;
	float3 EyePos : TEYEPOS;
	float3 geomNormal : NORMAL;
	float3x3 TBN : TBNMATRIX;
	uint instanceID : INSTANCEID;
};

// Vertex shader
VSOut VSMain(VSIn input)
{
	VSOut output;

	float4x4 instanceMat = float4x4(input.rowX, input.rowY, input.rowZ, input.rowW);
	output.TBN = (float3x3(input.tangent, input.bitangent, input.normal));
	float4x4 modelMat = mul(g_meshMat, instanceMat);
	
	output.geomNormal = normalize(input.normal);

	output.EyePos = (g_cameraPos);

	output.PosWS = mul(float4(input.position, 1.0f), modelMat).xyz;
	output.projPos = mul(float4(output.PosWS, 1.0f), g_viewProj);
	output.texCoord = input.texCoord * input.texCoordScale;
	output.instanceID = input.instanceID;
	return output;
}

// Pixel shader
cbuffer cbPerObject : register(b2)
{
	Material g_material;
};

Texture2D objTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D roughnessTexture : register(t2);
Texture2D metallicTexture : register(t3);

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

	//output.geomNormal = input.geomNormal;
	output.albedo = objTexture.Sample(g_sampler, input.texCoord);

	float3 normal;
	if (g_material.hasNormalTexture)
		normal = normalize(normalTexture.Sample(g_sampler, input.texCoord).rgb * 2.0f - 1.0f);
	else
		normal = input.geomNormal;

	normal = normalize(mul(normal, input.TBN));

	output.normalPacked = packOctahedron(swapYZ(normal));

	if (g_material.hasMetallicTexture)
		output.roughMetal.y = metallicTexture.Sample(g_sampler, input.texCoord).r;
	else
		output.roughMetal.y = g_material.metalness;

	if (g_material.hasRoughnessTexture)
		output.roughMetal.x = roughnessTexture.Sample(g_sampler, input.texCoord).r;
	else
		output.roughMetal.x = g_material.roughness;

	output.emission = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.objectID =  input.instanceID;

	return output;
}
