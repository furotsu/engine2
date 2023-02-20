#include "pbrRes.hlsli"


StructuredBuffer<DisappearanceParticle> particlesData;
Buffer<uint> particlesRange; //[0] - begin; [1] - count; [2] - deathCount

//Particle vertex shader
struct VSOut
{
	float4 projPos : SV_POSITION;
	float2 texCoords : TEXCOORD;
	float alpha : ALPHA;
};

// Vertex shader
VSOut VSMain(uint inputInstanceID : SV_InstanceID, uint inputVertexID : SV_VertexID)
{
	VSOut output;
	DisappearanceParticle particle = particlesData[(particlesRange[0] + inputInstanceID) % g_maxParticles];
	float2 size = float2(0.2f, 0.2f);
	float3 cameraRight = float3(g_view[0][0], g_view[1][0], g_view[2][0]);
	float3 cameraUp = float3(g_view[0][1], g_view[1][1], g_view[2][1]);
	float3 cameraFront = float3(g_view[0][2], g_view[1][2], g_view[2][2]);

	float3 position = particle.pos;

	float3 rectVert[4] = { {-0.5f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f} , {0.5f, 0.5f, 0.0f} , {0.5f, -0.5f, 0.0f} };
	//offseting each vertex of rectangle by view matrix vectors to make particle always face camera	
	float3 vertexPos[4] = { {position + size.x * cameraRight * rectVert[0].x + size.y * cameraUp * rectVert[0].y},
							{position + size.x * cameraRight * rectVert[1].x + size.y * cameraUp * rectVert[1].y},
							{position + size.x * cameraRight * rectVert[2].x + size.y * cameraUp * rectVert[2].y},
							{position + size.x * cameraRight * rectVert[3].x + size.y * cameraUp * rectVert[3].y} };


	float vertexId[6] = { 0, 1, 2, 0, 2, 3 };

	float2 texCoords[4] = { {0.0f, 1.0f},
					{0.0f, 0.0f},
					{1.0f, 0.0f},
					{1.0f , 1.0f} };

	output.texCoords = texCoords[vertexId[inputVertexID]];
	output.projPos = mul(float4(vertexPos[vertexId[inputVertexID]], 1.0f), g_viewProj);
	output.alpha = particle.alpha;
	return output;
}

Texture2D g_sparkTexture : register(t2);

// Particle pixel shader
float4 PSMain(VSOut input) : SV_TARGET
{
	float texColor = g_sparkTexture.Sample(g_sampler, input.texCoords);
	
	if (texColor < 0.1f)
		discard;

	return float4(texColor * float3(150.0f, 20.0f, 5.0f), texColor * input.alpha);
}

