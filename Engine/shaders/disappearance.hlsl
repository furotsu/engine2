#include "pbrRes.hlsli"

cbuffer cbPerObject : register(b2)
{
	row_major float4x4 g_meshMat;
};

struct VSIn
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float3 normal : NORMAL;
	float4 rowX : ROWX;
	float4 rowY : ROWY;
	float4 rowZ : ROWZ;
	float4 rowW : ROWW;
	float texCoordScale : TSCALE;
	float startTimePoint : TIMEPOINT;
	float3 spherePos : SPHEREPOS;
	float sphereGrowthSpeed : SPHEREGROWTHSPEED;
	uint  instanceID : INSTANCEID;
};

struct VSOut
{
	float4 projPos : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 PosWS  : POSITION;
	float3 EyePos : TEYEPOS;
	float3 geomNormal : GEOMNORMAL;
	float3x3 TBN : M;
	float startTimePoint : TIMEPOINT;
	float3 spherePos : SPHEREPOS;
	float sphereGrowthSpeed : SPHEREGROWTHSPEED;
	uint instanceID : INSTANCEID;
};

// Vertex shader
VSOut VSMain(VSIn input)
{
	VSOut output;

	float4x4 instanceMat = float4x4(input.rowX, input.rowY, input.rowZ, input.rowW);
	output.TBN = (float3x3(input.tangent, input.bitangent, input.normal));
	float4x4 modelMat = mul(g_meshMat, instanceMat);
	float3x3 TBN = transpose(float3x3(input.tangent, input.bitangent, input.normal));

	output.geomNormal = normalize(input.normal);

	float3 posWS = mul(float4(input.position, 1.0f), modelMat).xyz;
	output.EyePos = ((g_cameraPos));

	output.PosWS = posWS;
	output.projPos = mul(float4(posWS, 1.0f), g_viewProj);
	output.texCoord = input.texCoord * input.texCoordScale;
	output.instanceID = input.instanceID;
	output.spherePos = input.spherePos;
	output.sphereGrowthSpeed = input.sphereGrowthSpeed;
	output.startTimePoint = input.startTimePoint;
	return output;
}

// Pixel shader

cbuffer cbPerObject : register(b2)
{
	Material g_material;
	float g_spawnTime;
	float3 paddingDissol;
};

Texture2D objTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D roughnessTexture : register(t2);
Texture2D metallicTexture : register(t3);
Texture2D g_perlinNoise : register(t4);

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
	output.objectID = input.instanceID;

	
	float3 lineColor = float3(0.9f, 0.3f, 0.02f);
	float deltaTime = (g_timePoint - input.startTimePoint);
	
	float noiseValue = g_perlinNoise.Sample(g_sampler, input.texCoord).r;
	
	float radius = (g_timePoint - input.startTimePoint) * input.sphereGrowthSpeed;
	float lineWidth = 0.1f;
	float dist = length(input.spherePos - input.PosWS);
	if (dist < radius + lineWidth * noiseValue)
	{
		if (dist > radius - lineWidth * noiseValue)
		{
			output.emission = float4(lineColor.xyz, 1.0f);
		}
		else
			discard;
	}

	return output;
}


