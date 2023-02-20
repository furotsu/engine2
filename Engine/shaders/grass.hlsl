#include "pbrRes.hlsli"

struct VSIn
{
	float3 position : POSITION;
	float2 size : SIZE;
	uint vertexID : SV_VertexID;
	uint instanceID : SV_InstanceID;
};

struct VSOut
{
	float4 projPos : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 PosWS  : POSITION;
	float3 EyePos : TEYEPOS;
	float3x3 TBN : TBN;
};

cbuffer cbPerObject : register(b2)
{
	row_major float4x4 g_viewProjMat[6];
	uint g_textureCubePos;
	float3 paddingGeom;
};

float computeGrassAngle(float2 instancePos, float2 windDir); // windDir may be read from 2x2 wind matrix


// Vertex shader
VSOut VSMain(VSIn input)
{
	const int BUSHES_AMOUNT = 4;
	int separationNum = 4;

	uint vertNum = BUSHES_AMOUNT * 2 * (1 + separationNum);
	float3 cameraUp = float3(0.0f, 1.0f, 0.0f);

	float2 windDir = normalize(float2(1.0f, 1.0f));
	float grassAngle = computeGrassAngle(input.position.xz, windDir);

	float3 windZ = cross(float3(windDir.x, 0.0f, windDir.y), float3(0.0f, 1.0f, 0.0f));

	float3x3 windRot = { float3(windDir.x, 0.0f, windDir.y), float3(0.0f, 1.0f, 0.0f), windZ };
	float3x3 windRotInv = transpose(windRot);

	float3 vertexPos[24];
	float3 startNormals[4] = { {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, normalize(float3(1.0f, 0.0f, -1.0f)), normalize(float3(-1.0f, 0.0f, -1.0f)) };
	float3 startNormal = startNormals[input.instanceID % 4];
	float3 normals[24];

	float R = input.size.y / (max(0.000000001f, grassAngle));
	float3 worldAxises[4] = { {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, normalize(float3(1.0f, 0.0f, 1.0f)), normalize(float3(1.0f, 0.0f, -1.0f)) };
	float3 worldAxis = worldAxises[input.instanceID % 4];

	float sinArray[4]; //size = separationNum
	float cosArray[4];
	for (int k = 1; k != separationNum + 1; k++)
	{
		float angle = grassAngle / separationNum * k;
		sincos(angle, sinArray[k - 1], cosArray[k - 1]);
	}

	for (int i = 0; i != 1; i++)
	{
		vertexPos[0] = input.position + worldAxis * -input.size.x / 2.0f;
		vertexPos[1] = input.position + worldAxis * input.size.x / 2.0f;
		normals[0] = startNormal;
		normals[1] = startNormal;
		for (int j = 1; j != separationNum + 1; j++)
		{
			float3 normal = mul(startNormal, windRot);
			float sinAngle = sinArray[j - 1];
			float cosAngle = cosArray[j - 1];
			float3 left = worldAxis * (-input.size.x / 2.0f);
			float3 right = worldAxis * (+input.size.x / 2.0f);

			left = mul(left, windRot);
			right = mul(right, windRot);

			left.x += R;
			right.x += R;

			float xLeft = cosAngle * (left.x) - sinAngle * (left.y);
			float yLeft = sinAngle * (left.x) + cosAngle * (left.y);
			float xRight = cosAngle * (right.x) - sinAngle * (right.y);
			float yRight = sinAngle * (right.x) + cosAngle * (right.y);

			float xNormal = cosAngle * (normal.x) - sinAngle * (normal.y);
			float yNormal = sinAngle * (normal.x) + cosAngle * (normal.y);

			normal = float3(xNormal, yNormal, normal.z);

			left.x = xLeft - R;    left.y = yLeft;
			right.x = xRight - R;  right.y = yRight;

			left = mul(left, windRotInv);
			right = mul(right, windRotInv);
			normal = normalize(mul(normal, windRotInv));

			int id = (j - 1) * 2 + 2;
			normals[id] = normal;
			normals[id + 1] = normal;

			vertexPos[id] = float3(input.position.x, input.position.y, input.position.z) + left;
			vertexPos[id + 1] = float3(input.position.x, input.position.y, input.position.z) + right;
		}
	}

	float texCoordPart = 1.0f / (separationNum );
	float2 texCoords[4] = { {0.0f, 1.0f},
							 {1.0f, 1.0f},
							{0.0f, 1.0f - texCoordPart},
							{ 1.0f, 1.0f - texCoordPart}};

	float vertex[6] = { 0, 2, 3, 0, 3, 1 };
	float texCoord[6] = { 0, 2, 3, 0, 3, 1 };


	VSOut output = (VSOut)0;
	int vertexID = vertex[input.vertexID % 6] + 2 * (input.vertexID / 6);
	output.PosWS = vertexPos[vertexID];

	
	float3 normal = normalize(normals[vertexID]);
	float3 bitangent = cross(normal, worldAxis);
	output.TBN = float3x3(worldAxis, bitangent, normal);
	output.texCoord = texCoords[vertex[input.vertexID % 6]] - float2(0.0f, texCoordPart * (uint)(input.vertexID / 6));

	output.projPos = mul(float4(output.PosWS, 1.0f), g_viewProj);
	
	output.EyePos = (g_cameraPos);

	return output;
}

// Pixel shader
cbuffer cbPerObject : register(b2)
{
	Material g_material;
};

Texture2D g_grassTexture : register(t0);
Texture2D g_normalTexture : register(t1);
Texture2D g_opacityTexture : register(t2);
TextureCubeArray shadowMap : register(t4);
TextureCube g_irradiance : register(t5);
TextureCube g_reflection : register(t6);
Texture2D g_reflectance : register(t7);

struct PSOut
{
	float2 normalPacked : SV_Target0;
	float4 albedo: SV_Target1;
	float2 roughMetal: SV_Target2;
	float4 emission: SV_Target3;
	uint objectID: SV_Target4;
};

PSOut PSMain(VSOut input, in bool isFrontFace : SV_IsFrontFace) : SV_TARGET
{
	PSOut output;

	float3 texNormal = g_normalTexture.Sample(g_grassSampler, input.texCoord);
	float3 normal = mul(texNormal, input.TBN);
	
	//revert normal if it not fron-facing
	if (!isFrontFace)
	{
		normal = -normal;
	}

	output.normalPacked = packOctahedron(normal);

	float alpha = g_opacityTexture.Sample(g_grassSampler, input.texCoord).r;
	if (alpha < 0.2f)
		discard;
	output.albedo = float4(g_grassTexture.Sample(g_grassSampler, input.texCoord).xyz, 1.0f);
	
	output.roughMetal = float2(0.999f, 0.000001f);
	output.emission = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.objectID = g_grassID;
	return output;
}

float computeGrassAngle(float2 instancePos, float2 windDir) // windDir may be read from 2x2 wind matrix
{
	const float MAX_VARIATION = g_PI;
	float WIND_WAVE_LENGTH = 50;
	float POWER_WAVE_LENGTH = 233;
	const float WIND_OSCILLATION_FREQ = 0.666;
	const float POWER_OSCILLATION_FREQ = 1.0 / 8.0;

	float instanceRandom = frac(instancePos.x * 12345);
	float windCoord = dot(instancePos, windDir);

	float windPhaseVariation = instanceRandom * MAX_VARIATION;
	float windPhaseOffset = windCoord / WIND_WAVE_LENGTH + windPhaseVariation;
	float windOscillation = (sin(windPhaseOffset - WIND_OSCILLATION_FREQ * g_2PI * g_timePoint) + 1) / 2;

	float powerPhaseOffset = windCoord / POWER_WAVE_LENGTH;
	float powerOscillation = (sin(powerPhaseOffset - POWER_OSCILLATION_FREQ * g_2PI * g_timePoint) + 1) / 2;

	float minAngle = lerp(0.0, 0.3, powerOscillation);
	float maxAngle = lerp(0.1, 1.0, powerOscillation);
	return lerp(minAngle, maxAngle, windOscillation);
}