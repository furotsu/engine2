#ifndef __GLOBALS_HLSLI__
#define __GLOBALS_HLSLI__

#include "structures.hlsli"

static const float g_SMALL_OFFSET = 0.0001f;
static const float g_PI = 3.14f;
static const float g_2PI = g_PI * 2.0f;
static const float3 g_INSULATOR_F0 = float3(0.04f, 0.04f, 0.04f);
static const float g_MIN_ROUGHNESS = 0.000000001f;
static const uint g_MAX_POINT_LIGHTS = 5;

cbuffer PerFrame : register(b0)
{
	row_major float4x4 g_viewProj;
	row_major float4x4 g_view;
	row_major float4x4 g_viewInv;
	row_major float4x4 g_proj;
	row_major float4x4 g_projInv;
	row_major float4x4 g_LightProj;
	float3 g_cameraPos;
	float g_screenWidth;
	float g_screenHeight;
	float g_EV100;
	float g_pointLightsNum;
	float g_reflectionMips;
	float2 g_nearFarPlanes;
	float g_pointlightShadowResolution;
	float g_timePoint;
	float g_frameDeltaTime;
	uint g_maxParticles;
	uint g_grassID;
	float g_dissolutionSpawnTime;
};

cbuffer PerFrameLights : register(b1)
{
	PointLight g_pointLight[g_MAX_POINT_LIGHTS];
	row_major float4x4 g_pointLightViewProj[g_MAX_POINT_LIGHTS * 6];
};

SamplerState g_sampler : register(s0);
SamplerComparisonState g_shadowSampler : register(s1);
SamplerState g_linearClampSampler : register(s2);
SamplerState g_grassSampler : register(s3);

#endif // __GLOBALS_HLSLI__