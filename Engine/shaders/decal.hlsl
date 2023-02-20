#include "globals.hlsli"
#include "pbrRes.hlsli"

// Vertex shader
struct VSIn
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
	float4 rowX : ROWX;
	float4 rowY : ROWY;
	float4 rowZ : ROWZ;
	float4 rowW : ROWW;
	float4 inverseRowX : INVROWX;
	float4 inverseRowY : INVROWY;
	float4 inverseRowZ : INVROWZ;
	float4 inverseRowW : INVROWW;
	float3 albedo : ALBEDO;
	float roughness : ROUGHNESS;
	float metalness: METALNESS;
	uint objID : OBJID;
};

struct VSOut
{
	float4 projPos : SV_POSITION;
	float4 PosWS : POSITION;
	float3 albedo : ALBEDO;
	float2 roughMetal : ROUGHMETAL;
	float2 texCoord : TEXCOORD;
	float4x4 inverseInstanceMat : INVMAT;
	uint objID : OBJID;
};

VSOut VSMain(VSIn input, uint instanceId : SV_InstanceID)
{
	VSOut output;

	float4x4 instanceMat = float4x4(input.rowX, input.rowY, input.rowZ, input.rowW);

	output.PosWS = mul(float4(input.position, 1.0f), instanceMat);

	output.inverseInstanceMat = float4x4(input.inverseRowX, input.inverseRowY, input.inverseRowZ, input.inverseRowW);
	
	output.projPos = mul(float4(output.PosWS.xyz, 1.0f), g_viewProj);
	output.texCoord = input.texCoord;
	output.albedo = input.albedo;
	output.roughMetal = float2(input.roughness, input.metalness);
	output.objID = input.objID;
	return output;
}

//Pixel shader
struct PSOut
{
	float2 normalPacked : SV_Target0;
	float4 albedo: SV_Target1;
	float2 roughMetal: SV_Target2;
	float4 emission: SV_Target3;
	uint objectID: SV_Target4;
};

Texture2D<float4> g_depthTexture : register(t0);
Texture2D<float4> g_normalsTexture: register(t1);
Texture2D<uint> g_objectIDTexture: register(t2);
Texture2D g_decalNormalTexture : register(t3);

PSOut PSMain(VSOut input) : SV_TARGET
{
	PSOut output;
	
	uint sceneObjID = g_objectIDTexture.Load(int3(input.projPos.xy, 0));

	if (input.objID != sceneObjID)
		discard;

	float2 screenTexCoords = input.projPos.xy / float2(g_screenWidth, g_screenHeight);
	float depth = g_depthTexture.Sample(g_sampler, screenTexCoords);
	float3 sceneNormal = unpackOctahedron(g_normalsTexture.Sample(g_sampler, screenTexCoords)).xzy;
	float4 pos = worldPosFromDepth(screenTexCoords, depth);

	float2 texCoords = mul(float4(pos.xyz, 1.0f), input.inverseInstanceMat);

	texCoords.xy += 0.5f;

	float3 cameraRight = float3(input.inverseInstanceMat[0][0], input.inverseInstanceMat[1][0], input.inverseInstanceMat[2][0]);

	float4 normal = g_decalNormalTexture.Sample(g_linearClampSampler, saturate(texCoords.xy));
	normal.xyz = normal.xyz * 2.0f - 1.0f;
	if (normal.a <= 0.5f)
		discard;

	float3 tangent = normalize(cameraRight - sceneNormal * dot(sceneNormal, cameraRight));
	float3 bitangent = cross(sceneNormal, tangent);
	float3x3 TBN = float3x3(tangent, bitangent, sceneNormal.xyz);

	normal.xyz = mul(normal.xyz, TBN).xyz;
	normal.xyz = swapYZ(normal);
	normal.y = -normal.y;
	output.albedo = float4(input.albedo, 1.0f);
	output.normalPacked = packOctahedron(normalize(normal.xyz ).xyz);
	output.roughMetal = input.roughMetal;
	output.emission = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.objectID = input.objID;

	return output;

}
