#ifndef __STRUCTURES_HLSLI__
#define __STRUCTURES_HLSLI__


struct DirLight
{
	float3 color;
	float solidAngle;
	float3 dir;
	float power;
};

struct PointLight
{
	float3 pos;
	float radius;
	float3 color;
	float padding1;
	float3 radiance;
	float padding2;
};

struct SpotLight
{
	float3 pos;
	float radius;
	float3 dir;
	float innerCutOff;
	float3 radiance;
	float outerCutOff;
	float3 color;
	float padding;
};

struct Material
{
	float roughness;
	float metalness;
	float hasDiffuseTexture;
	float hasNormalTexture;
	float hasRoughnessTexture;
	float hasMetallicTexture;
	float reverseNormalTextureY;
	float padding3;
};

struct	Surface
{
	float roughness;
	float3 normal;
	float3 geomNormal;
	float3 metalness;
	float3 albedo;
	float3 F0;
};

struct View
{
	float3 reflectionDir;
	float NoV;
};

struct IBLTextures
{
	TextureCube irradiance;
	TextureCube reflection;
	Texture2D   reflectance;
	float reflectionMips;
};

struct DisappearanceParticle
{
	float3 pos;
	float spawnTime;
	float3 velocity;
	float alpha;
};

#endif // __STRUCTURES_HLSLI__