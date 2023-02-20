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
	uint vertexID : SV_VertexID;
};

uniform RWStructuredBuffer<DisappearanceParticle> particlesData : register(u0);
uniform RWBuffer<uint> particlesRange : register(u1); //[0] - begin; [1] - count; [2] - deathCount
uniform RWBuffer<uint> indirectArgsBuffer : register(u2); //[0] - VertexCountPerInstance; [1] - InstanceCount;
//[2] - StartVertexLocation; [3] - StartInstanceLocation;

// Vertex shader
void VSMainSpawner(VSIn input)
{
	float4x4 instanceMat = float4x4(input.rowX, input.rowY, input.rowZ, input.rowW);
	float4x4 modelMat = mul(g_meshMat, instanceMat);

	float3 PosWS = mul(float4(input.position, 1.0f), modelMat).xyz;
	float prevTime = g_timePoint - g_frameDeltaTime;

	float radius = (g_timePoint - input.startTimePoint) * input.sphereGrowthSpeed;
	float lineWidth = 0.5f;
	float dist = length(input.spherePos - PosWS);

	float spawnTime = (dist - lineWidth) / input.sphereGrowthSpeed + input.startTimePoint;

	if (prevTime < spawnTime && spawnTime < g_timePoint)
	{
		uint maxSimultaneousParticles = 5000;
		DisappearanceParticle newParticle;
		newParticle.pos = PosWS;
		newParticle.spawnTime = g_timePoint;
		newParticle.velocity = input.normal * 10.0f;
		newParticle.alpha = 0.1f;

		uint prevCount;
		
		InterlockedAdd(particlesRange[1], 0u, prevCount);

		if (prevCount < g_maxParticles - maxSimultaneousParticles)
		{
			InterlockedAdd(particlesRange[1], 1u, prevCount);
			indirectArgsBuffer[0] = 6;
			indirectArgsBuffer[2] = 0;
			indirectArgsBuffer[3] = 0;
			particlesData[(particlesRange[0] + prevCount) % g_maxParticles] = newParticle;
		}


	}

	return;
}

Texture2D g_depthTexture : register(t0);
Texture2D g_normalsTexture: register(t1);

[numthreads(64, 1, 1)]
void CSMainUpdater(uint3 ThreadID : SV_DispatchThreadID)
{
	DisappearanceParticle particle = particlesData[ThreadID.x];
	float3 gravity = float3(0.0f, -25.0f, 0.0f);
	float maxParticleLifeTime = 4.0f;

	float f1 = (particlesRange[1] + particlesRange[0]) % g_maxParticles;
	float f2 = particlesRange[0];
	bool flag = false;
	if (f1 >= f2)
	{
		flag = ThreadID.x < f2 || f1 <= ThreadID.x;
	}
	else
		flag = ThreadID.x < f2 && f1 <= ThreadID.x;

	if (flag)
	{
		return;
	}

	particle.velocity = particle.velocity + gravity * float3(0.0f, g_frameDeltaTime, 0.0f);
	particle.pos += particle.velocity * g_frameDeltaTime;

	float deltaTime = g_timePoint - particle.spawnTime;
	uint prevValue;

	if (deltaTime > maxParticleLifeTime)
	{
		InterlockedAdd(particlesRange[2], 1, prevValue);
	}

	//collisionCheck;
	float2 texCoords = texCoordsFromPosWS(particle.pos);

	float4x4 faceViewProj = g_viewProj;
	float4 posCS = mul(float4(particle.pos, 1.0), faceViewProj);
	float comparingDepth = posCS.z / posCS.w;

	float depth = g_depthTexture.SampleLevel(g_sampler, texCoords, 0);
	float3 normal = unpackOctahedron(g_normalsTexture.SampleLevel(g_sampler, texCoords, 0));
	float3 PosWS = worldPosFromDepth(texCoords, depth);

	if ((comparingDepth < depth) && length(PosWS - particle.pos) < 3.0f)
	{
		float3 v = reflect(particle.velocity, normal);

		particle.velocity = v * 0.7f;
		particle.pos += particle.velocity * g_frameDeltaTime;
		particle.pos += particle.velocity * g_frameDeltaTime;
	}

	if (deltaTime <= 0.2f)
		particle.alpha = lerp(particle.alpha, 0.2f, deltaTime);
	else if (deltaTime <= maxParticleLifeTime)
		particle.alpha = lerp(1.0f, 0.05f, deltaTime/(1.01f* maxParticleLifeTime));

	particlesData[ThreadID.x] = particle;
};

[numthreads(1, 1, 1)]
void CSMainRange(uint3 ThreadID : SV_DispatchThreadID)
{
	particlesRange[0] = (particlesRange[0] + particlesRange[2]) % g_maxParticles;
	particlesRange[1] -= particlesRange[2];
	particlesRange[2] = 0;
	indirectArgsBuffer[1] = particlesRange[1];
};

