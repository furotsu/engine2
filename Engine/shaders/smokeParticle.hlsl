
#include "pbrRes.hlsli"

struct VSIn
{
	float3 position : POSITION;
	float4 tint : TINT;
	float2 size : SIZE;
	float thickness : THICKNESS;
	float angle : ANGLE;
	float startTimePoint : TIMEPOINT;
	uint vertexId : SV_VertexID;
};

struct VSOut
{
	float4 projPos : SV_POSITION;
	float3 particlePos : PARTICLEPOS;
	float2 size : SIZE;
	float4 tint : TINT;
	float3 PosWS  : POSITION;
	float2 uvThis : TEXCOORD1;
	float2 uvNext : TEXCOORD2;
	float fracTime : FRACTIME;
	float3x3 rotMat : ROTMAT;
};

// Vertex shader
VSOut VSMain(VSIn input)
{
	VSOut output;

	float sinAngle;
	float cosAngle;
	sincos(input.angle, sinAngle, cosAngle);

	output.rotMat = float3x3(cosAngle, -sinAngle, 0.0f,
		sinAngle, cosAngle, 0.0f,
		0.0f, 0.0f, 1.0f);

	float3 cameraRight = float3(g_view[0][0], g_view[1][0], g_view[2][0]);
	float3 cameraUp = float3(g_view[0][1], g_view[1][1], g_view[2][1]);
	float3 cameraFront = float3(g_view[0][2], g_view[1][2], g_view[2][2]);
	float3 rectVert[4] = { {-0.5f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f} , {0.5f, 0.5f, 0.0f} , {0.5f, -0.5f, 0.0f} };

	rectVert[0] = mul(rectVert[0], output.rotMat);
	rectVert[1] = mul(rectVert[1], output.rotMat);
	rectVert[2] = mul(rectVert[2], output.rotMat);
	rectVert[3] = mul(rectVert[3], output.rotMat);

	sincos(-input.angle, sinAngle, cosAngle);


	output.rotMat = float3x3(cosAngle, -sinAngle, 0.0f,
		sinAngle, cosAngle, 0.0f,
		0.0f, 0.0f, 1.0f);

	output.rotMat = mul(g_view, mul(output.rotMat, g_viewInv));


	//offseting each vertex of rectangle by view matrix vectors to make particle always face camera	
	float3 vertexPos[4] = { {input.position + input.size.x * cameraRight * rectVert[0].x + input.size.y * cameraUp * rectVert[0].y},
							{input.position + input.size.x * cameraRight * rectVert[1].x + input.size.y * cameraUp * rectVert[1].y},
							{input.position + input.size.x * cameraRight * rectVert[2].x + input.size.y * cameraUp * rectVert[2].y},
							{input.position + input.size.x * cameraRight * rectVert[3].x + input.size.y * cameraUp * rectVert[3].y} };


	float vertexId[6] = { 0, 1, 2, 0, 2, 3 };

	output.particlePos = input.position;
	output.size = input.size;
	output.PosWS = vertexPos[vertexId[input.vertexId]];
	output.projPos = mul(float4(output.PosWS, 1.0f), g_viewProj);
	output.tint = input.tint;

	const float animTime = 5.0f;

	uint textureCells = 8;

	uint texPos = (textureCells * textureCells * (g_timePoint - input.startTimePoint)) / animTime;

	float2 texCoords[4] = { {0.0f, 1.0f / 8.0f},
						{0.0f, 0.0f},
						{1.0f / 8.0f, 0.0f},
						{1.0f / 8.0f, 1.0f / 8.0f} };


	//fraction time between two frames in atlas
	output.fracTime = saturate(((g_timePoint - input.startTimePoint) - (animTime / (textureCells * textureCells)) * (texPos))); 

	output.uvThis = float2((texPos % textureCells) / 8.0f, texPos / textureCells / 8.0f) + texCoords[vertexId[input.vertexId]];
	texPos += 1;
	output.uvNext = float2((texPos % textureCells) / 8.0f, texPos / textureCells / 8.0f) + texCoords[vertexId[input.vertexId]];

	return output;
}

Texture2D g_emissionTexture : register(t0);
Texture2D g_EMVATexture : register(t1);
Texture2D g_RLTTexture : register(t2);// - red is right, green is left, - blue is top
Texture2D g_BotBFTexture : register(t3); // - red is bottom, green is back, blue is front
TextureCubeArray shadowMap : register(t4);
Texture2D g_sceneDepth : register(t5);

// Pixel shader
float4 PSMain(VSOut input) : SV_TARGET
{
	const float g_mvScale = 0.001; // find such constant that frame transition becomes correct and smooth

	float2 mvA = g_EMVATexture.Sample(g_sampler, input.uvThis).gb;
	float2 mvB = g_EMVATexture.Sample(g_sampler, input.uvNext).gb;

	mvA = mvA * 2.0f - 1.0f;
	mvB = mvB * 2.0f - 1.0f;
	float2 uvA = input.uvThis; // this frame UV
	uvA -= mvA * g_mvScale * input.fracTime;
	float2 uvB = input.uvNext; // next frame UV
	uvB -= mvB * g_mvScale * (input.fracTime - 1.f);

	float4 emissionA = g_emissionTexture.Sample(g_sampler, uvA);
	float4 emissionB = g_emissionTexture.Sample(g_sampler, uvB);
	float4 emission = lerp(emissionA, emissionB, input.fracTime);

	float4 RLTA = g_RLTTexture.Sample(g_sampler, uvA);
	float4 RLTB = g_RLTTexture.Sample(g_sampler, uvB);
	float3 RLT = lerp(RLTA, RLTB, input.fracTime).xyz;
	float4 BotBFA = g_BotBFTexture.Sample(g_sampler, uvA);
	float4 BotBFB = g_BotBFTexture.Sample(g_sampler, uvB);

	float3 BotBF = lerp(BotBFA, BotBFB, input.fracTime);


	float4 res = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 cameraRight = float3(g_view[0][0], g_view[1][0], g_view[2][0]);
	float3 cameraUp = float3(g_view[0][1], g_view[1][1], g_view[2][1]);
	float3 cameraFront = float3(g_view[0][2], g_view[1][2], g_view[2][2]);
	float3 normal5;
	for (int i = 0; i != g_pointLightsNum; ++i)
	{
		float3 incomingLight = float3(0.0f, 0.0f, 0.0f);
		float3 lightDir = g_pointLight[i].pos - input.PosWS;

		float3 normLightDir = float3(0.0f, 0.0f, 0.0f);
		float distance = 0.0f;
		calcDirDistance(lightDir, normLightDir, distance);

		float solidAngle = computeSolidAngle(g_pointLight[i].radius, distance);
		float attenuation = solidAngle / (10.0f);

		float3 normal = normalize(-g_cameraPos);

		float shadow = getPointlightVisibility(cameraFront, normLightDir, input.PosWS, i, shadowMap);
		
		//rotate xy light direction instead of normals
		normLightDir = mul(normLightDir, input.rotMat);

		float LdotRight = dot(cameraRight, normLightDir);
		float LdotTop = dot(cameraUp, normLightDir);
		float LdotBack = dot(cameraFront, normLightDir);
		
		incomingLight +=  input.tint.xyz * shadow * g_pointLight[i].color * g_pointLight[i].radiance * attenuation * abs(LdotRight) * (LdotRight >= 0.0 ? RLT.r : RLT.g);
		incomingLight +=  input.tint.xyz * shadow * g_pointLight[i].color * g_pointLight[i].radiance * attenuation * abs(LdotTop) * (LdotTop >= 0.0 ? RLT.b : BotBFB.r);
		incomingLight +=  input.tint.xyz * shadow * g_pointLight[i].color * g_pointLight[i].radiance * attenuation * abs(LdotBack) * (LdotBack >= 0.0 ? BotBFB.g : BotBFB.b);
		res.xyz += incomingLight;
	}

	float4x4 faceViewProj = g_viewProj;
	float4 posCS = mul(float4(input.PosWS, 1.0), faceViewProj); // NOTE only 3rd and 4th rows are used
	float comparingDepth = posCS.z / posCS.w;
	float2 worldTexCoords = input.projPos.xy / float2(g_screenWidth, g_screenHeight);
	float particleDepth = g_sceneDepth.Sample(g_sampler, worldTexCoords);
	float3 comparingPos = worldPosFromDepth(worldTexCoords, particleDepth);
	
	float particleFading = saturate(length(input.PosWS - comparingPos) / input.size.x);
	res.xyz += emission.xyz;

	if (particleDepth > comparingDepth)
		discard;

	float alphaA = g_EMVATexture.Sample(g_sampler, uvA).a;
	float alphaB = g_EMVATexture.Sample(g_sampler, uvB).a;
	float alpha = lerp(alphaA, alphaB, input.fracTime);
	res.a = alpha * particleFading * input.tint.a;

	return res;
}
