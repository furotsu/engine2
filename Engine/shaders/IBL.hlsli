#ifndef __IBL_HLSLI__
#define __IBL_HLSLI_

#include "globals.hlsli"
#include "pbrRes.hlsli"
#include "structures.hlsli"


void addEnvironmentReflection(inout float3 diffuseReflection, inout float3 specularReflection, in View v, in Surface s, in IBLTextures t)
{
	diffuseReflection += s.albedo * (1.0 - s.metalness) * t.irradiance.SampleLevel(g_linearClampSampler, s.normal, 0.0f);

	float2 reflectanceLUT = t.reflectance.SampleLevel(g_linearClampSampler, float2(s.roughness, 1.0f - v.NoV), 0.0f);
	float3 reflectance = reflectanceLUT.x * s.F0 + reflectanceLUT.y;
	specularReflection += reflectance * t.reflection.SampleLevel(g_linearClampSampler, v.reflectionDir, s.roughness * g_reflectionMips);
}


#endif