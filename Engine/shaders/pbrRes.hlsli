#ifndef __PBRRES_HLSLI__
#define __PBRRES_HLSLI__

#include "globals.hlsli"
#include "IBL.hlsli"


// Schlick's approximation of Fresnel reflectance
float3 frensel(float NdotL, float3 F0)
{
	return F0 + (float3(1.0f, 1.0f, 1.0f) - F0) * pow(1 - NdotL, 5.0);
}

// Height-correlated Smith G2 for GGX,
// Filament, 4.4.2 Geometric shadowing
float smith(float rough2, float NoV, float NoL)
{
	NoV *= NoV;
	NoL *= NoL;
	return 2.0f / (sqrt(1.0f + rough2 * (1 - NoV) / NoV) + sqrt(1.0f + rough2 * (1 - NoL) / NoL));
}

// GGX normal distribution,
// Real-Time Rendering 4th Edition, page 340, equation 9.41
float ggx(float rough2, float NoH, float LightAngleSin = 0, float lightAngleCos = 0)
{
	float denom = NoH * NoH * (rough2 - 1.0f) + 1.0f;
	denom = g_PI * denom * denom;
	return rough2 / denom;
}

// calculate attenuation as solidAgnle / (2.0f * M_PI)
float computeSolidAngle(float radius, float distance)
{
	distance = max(distance, radius);
	float h = (1.0f - sqrt(1.0f - (radius / distance) * (radius / distance))) * 2.0f * g_PI;

	return h;
}

// May return direction pointing beneath surface horizon (dot(N, dir) < 0), use clampDirToHorizon to fix it.
// sphereCos is cosine of light sphere angular diameter.
// sphereRelPos is position of a sphere relative to surface:
// 'sphereDir == normalize(sphereRelPos)' and 'sphereDir * sphereDist == sphereRelPos'
float3 approximateClosestSphereDir(bool intersects, float3 reflectionDir, float sphereCos,
	float3 sphereRelPos, float3 sphereDir, float sphereDist, float sphereRadius)
{
	float RoS = dot(reflectionDir, sphereDir);

	intersects = (RoS >= sphereCos);
	if (intersects) { return reflectionDir; }
	if (RoS < 0.0f) return sphereDir;
	float3 closestPointDir = normalize(reflectionDir * sphereDist * RoS - sphereRelPos);
	return normalize(sphereRelPos + sphereRadius * closestPointDir);
}

// Input dir and NoD is L and NoL in a case of lighting computation 
void clampDirToHorizon(float3 dir, float NoD, float3 normal, float minNoD)
{
	if (NoD < minNoD)
	{
		dir = normalize(dir + (minNoD - NoD) * normal);
		NoD = minNoD;
	}
}

float3 swapYZ(float3 vec)
{
	float3 res = vec;
	res.z = vec.y;
	res.y = -vec.z;
	return res;
}

float3 CookTorrance_GGX(float3 viewDir, float3 fragNorm, float solidAngle, float3 lightDir, float3 F0, float distance, float radius, float rough2)
{
	float3 reflectionDir = reflect(viewDir, fragNorm);
	bool intersects = false;

	distance = max(distance, radius);
	float sphereCos = sqrt(1.0f - (radius / distance) * (radius / distance));

	float3 lightSpecDirection = approximateClosestSphereDir(intersects, -reflectionDir, sphereCos, lightDir * distance,
		lightDir, distance, radius);

	float NoL = max(dot(fragNorm, lightSpecDirection), g_SMALL_OFFSET);

	clampDirToHorizon(lightSpecDirection, NoL, fragNorm, g_SMALL_OFFSET);

	float3 halfDir = normalize(lightSpecDirection + viewDir);
	float NoH = max(dot(fragNorm, halfDir), g_SMALL_OFFSET);
	float LoH = max(dot(lightSpecDirection, halfDir), g_SMALL_OFFSET);
	float NoV = max(dot(fragNorm, viewDir), g_SMALL_OFFSET);

	float3 Fspec = saturate(frensel(LoH, F0));
	float D = ggx(rough2, NoH);
	float G = smith(rough2, NoV, NoL);
	float3 spec = Fspec * G * min(1.0f, solidAngle * D * 0.25f / (NoV));
	return spec;
}

void calcDirDistance(in float3 vec, inout float3 dir, inout float distance)
{
	dir = normalize(vec);
	distance = length(vec);
}

uint selectCubeFace(float3 unitDir)
{
	float maxVal = max(abs(unitDir.x), max(abs(unitDir.y), abs(unitDir.z)));
	uint index = abs(unitDir.x) == maxVal ? 0 : (abs(unitDir.y) == maxVal ? 2 : 4);
	return index + (asuint(unitDir[index / 2]) >> 31);
}

float2 texCoordsFromPosWS(float3 posWS)
{
	float4 projPos = mul(float4(posWS, 1.0f), g_viewProj);
	float2 texCoords = projPos.xy / projPos.w;

	texCoords.x = texCoords.x * 0.5f + 0.5f;
	texCoords.y = -texCoords.y * 0.5f + 0.5f;
	return texCoords;
}

// this is supposed to get the world position from the depth buffer
float4 worldPosFromDepth(float2 texCoords, float depth)
{
	texCoords.x = (2.0f * texCoords.x) - 1.0f;
	texCoords.y = 1.0f - (2.0f * texCoords.y);

	float4 clipSpacePosition = float4(texCoords, depth, 1.0);
	float4 viewSpacePosition = mul(clipSpacePosition, g_projInv);

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	float4 worldSpacePosition = mul(viewSpacePosition, g_viewInv);

	return worldSpacePosition;
}

float vectorToDepth(float3 vec, float n, float f)
{
	float3 AbsVec = abs(vec);
	float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

	float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
	return (NormZComp + 1.0) * 0.5;
}

float2 nonZeroSign(float2 v)
{
	return float2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0);
}

float2 packOctahedron(float3 v)
{
	float2 p = v.xy / (abs(v.x) + abs(v.y) + abs(v.z));
	return v.z <= 0.0 ? (float2(1.0f, 1.0f) - abs(p.yx)) * nonZeroSign(p) : p;
}

float3 unpackOctahedron(float2 oct)
{
	float3 v = float3(oct, 1.0 - abs(oct.x) - abs(oct.y));
	if (v.z < 0) v.xy = (float2(1.0f, 1.0f) - abs(v.yx)) * nonZeroSign(v.xy);
	return normalize(v);
}

float getPointlightVisibility(float3 N, float3 L, float3 posWS, uint pointlightIndex, TextureCubeArray shadowMap)
{
	const float DEPTH_OFFSET = 0.005;
	posWS += L * DEPTH_OFFSET; // affects comparingDepth, but not sampleDir

	float4x4 faceViewProj = g_pointLightViewProj[pointlightIndex * 6 + selectCubeFace(-L)];
	float4 posCS = mul(float4(posWS, 1.0), faceViewProj); // NOTE only 3rd and 4th rows are used
	float comparingDepth = posCS.z / posCS.w;

	float linearDepth = posCS.w; // distance along face Z
	float texelWorldSize = linearDepth * 2.0 / g_pointlightShadowResolution;

	posWS += N * texelWorldSize; // affects sampleDir, but not comparingDepth
	float3 sampleDir = posWS - g_pointLight[pointlightIndex].pos;

	float visibility = shadowMap.SampleCmp(g_shadowSampler, float4(sampleDir, pointlightIndex), comparingDepth);

	return visibility;
}

float3 calcPointLightPBR(PointLight light, Surface surface, float distance, float3 viewDir, float3 lightDir, float shadow)
{
	float solidAngle = computeSolidAngle(light.radius, distance);
	float attenuation = solidAngle / (2.0f * g_PI);
	float NoL = max(dot(surface.normal, lightDir), g_SMALL_OFFSET);
	float GnoL = dot(surface.geomNormal, lightDir);
	
	float lightMicroHeight = NoL * distance; // or = dot(N, lightDelta); // or = dot(N, lightDir) * lightDist;
	float lightMacroHeight = GnoL * distance; // GN is geometry normal, not the texture normal

	float fadingMicro = saturate((lightMicroHeight + light.radius) / (2.0 * light.radius));
	float fadingMacro = saturate((lightMacroHeight + light.radius) / (2.0 * light.radius));

	float lightDiameteralSin = min(light.radius / distance, 1.0f);

	NoL = max(NoL, fadingMicro * lightDiameteralSin); // fixes diffuse reflection when light sphere goes under horizon

	float rough2 = surface.roughness * surface.roughness;
	rough2 = max(rough2, g_MIN_ROUGHNESS);
	
	float3 spec = CookTorrance_GGX(viewDir, surface.normal, solidAngle, lightDir, surface.F0, distance, light.radius, rough2);
	
	float3 Fdiff = float3(1.0f, 1.0f, 1.0f) - frensel(NoL, surface.F0);
	float3 diff = (Fdiff * surface.albedo * light.color * attenuation) / (g_PI);

	return shadow * (diff * NoL + spec) * (light.radiance) * fadingMicro * fadingMacro;
}

float3 calcGrassTransmission(PointLight light, float NdotL, float distance)
{
	float solidAngle = computeSolidAngle(light.radius, distance);
	float attenuation = solidAngle / (2.0f * g_PI * g_PI);
	float3 transmission = attenuation * light.radiance * light.color * pow(-NdotL, 8.0f);
	return transmission;
}

#endif // __PBRRES_HLSLI__