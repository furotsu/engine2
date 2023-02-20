#include "globals.hlsli"
#include "pbrRes.hlsli"

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
Texture2D g_depthTexture : register(t0);
Texture2D g_normalsTexture: register(t1);
Texture2D g_albedoTexture: register(t2);
Texture2D g_roughMetalTexture: register(t3);
Texture2D g_emissionTexture: register(t4);
Texture2D<uint> g_objectIDTexture: register(t5);
TextureCubeArray shadowMap : register(t6);
TextureCube g_irradiance : register(t7);
TextureCube g_reflection : register(t8);
Texture2D g_reflectance : register(t9);

struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};


float4 PSMain(PSIn input) : SV_TARGET
{
	Surface surface;
	int2 samplePos = int2(input.texCoord.x * g_screenWidth, input.texCoord.y * g_screenHeight);

	float depth = g_depthTexture.Sample(g_sampler, input.texCoord);

	float2 roughMetal = g_roughMetalTexture.Sample(g_sampler, input.texCoord);
	float4 emission = g_emissionTexture.Sample(g_sampler, input.texCoord);
	uint objID = g_objectIDTexture.Load(int3(samplePos.xy, 0));
	surface.albedo = g_albedoTexture.Sample(g_sampler, input.texCoord).xyz;
	surface.normal = unpackOctahedron(g_normalsTexture.Sample(g_sampler, input.texCoord).xy);
	
	surface.roughness = roughMetal.x;
	surface.metalness = roughMetal.y;
	surface.geomNormal = surface.normal;
	surface.F0 = lerp(g_INSULATOR_F0, surface.albedo, surface.metalness);

	float3 res = float3(0.0f, 0.0f, 0.0f);

	float3 PosWS = worldPosFromDepth(input.texCoord, depth);
	
	float3 viewDir = normalize(g_cameraPos - PosWS);
	
	for (int i = 0; i != g_pointLightsNum; ++i)
	{
		float3 lightDir = g_pointLight[i].pos - PosWS;
		float3 normLightDir = float3(0.0f, 0.0f, 0.0f);
		float distance = 0.0f;
		calcDirDistance(lightDir, normLightDir, distance);

		float shadow = getPointlightVisibility(surface.normal, lightDir, PosWS, i, shadowMap);
		res += calcPointLightPBR(g_pointLight[i], surface, distance, viewDir, normLightDir, shadow);
	}
	//calculate IBL component
	IBLTextures ibl;
	ibl.irradiance = g_irradiance;
	ibl.reflection = g_reflection;
	ibl.reflectance = g_reflectance;

	float3 diffIBL = float3(0.0f, 0.0f, 0.0f);
	float3 specIBL = float3(0.0f, 0.0f, 0.0f);
	View v;
	v.NoV = max(dot(surface.normal, viewDir), g_SMALL_OFFSET);
	v.reflectionDir = -reflect(viewDir, surface.normal);
	addEnvironmentReflection(diffIBL, specIBL, v, surface, ibl);
	res += diffIBL + specIBL;
	res += emission;

	return float4(res, 1.0f);
}
