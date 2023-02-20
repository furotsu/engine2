#include <algorithm>
#include <utility>

#include "modelManager.hpp"
#include "shaderManager.hpp"
#include "particleSystem.hpp"
#include "meshSystem.hpp"
#include "constants.hpp"

namespace engine
{
	ParticleSystem* ParticleSystem::s_manager = nullptr;

	ParticleSystem::ParticleSystem()
	{
		std::vector<ShaderInfo> shadersSmoke = {
			{ShaderType::VERTEX, L"shaders/smokeParticle.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/smokeParticle.hlsl",  "PSMain"}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedSmoke =
		{
			{"POSITION",   0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TINT",   0,      DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"SIZE",   0,      DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"THICKNESS",   0, DXGI_FORMAT_R32_FLOAT, 0, 36, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ANGLE", 0,       DXGI_FORMAT_R32_FLOAT,  0, 40, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TIMEPOINT", 0,   DXGI_FORMAT_R32_FLOAT,  0, 44, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		m_smokeShader = ShaderManager::GetInstance()->getShader(shadersSmoke, iedSmoke);

		particleInstanceBuffer.init(MAX_PARTICLE_NUM * sizeof(engine::SmokeParticle::Properties), D3D11_USAGE_DYNAMIC);

		initDisappearanceParticlesData();
	}

	void ParticleSystem::init()
	{

		if (s_manager == nullptr)
		{
			s_manager = new ParticleSystem();
		}
		else
		{
			ASSERT(false && "Initializing \" TransformSystem \" singleton more than once ");
		}
	}

	void ParticleSystem::deinit()
	{
		if (s_manager == nullptr)
		{
			ASSERT(false && "Trying to delete \" TransformSystem \" singleton more than once ");
		}
		else
		{
			clean();
			delete s_manager;
			s_manager = nullptr;
		}
	}

	ParticleSystem* ParticleSystem::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ERROR("Trying to call \" TransformSystem \" singleton  instance before initializing it");
		}
#endif
		return s_manager;
	}

	void ParticleSystem::initDisappearanceParticlesData()
	{
		/////
		// Create Structured Buffer
		D3D11_BUFFER_DESC sbDesc;
		ZeroMemory(&sbDesc, sizeof(D3D11_BUFFER_DESC));
		sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		sbDesc.CPUAccessFlags = 0;
		sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		sbDesc.StructureByteStride = sizeof(XMVECTOR) * 2;
		sbDesc.ByteWidth =  MAX_PARTICLE_NUM * 2 * sizeof(XMVECTOR);
		sbDesc.Usage = D3D11_USAGE_DEFAULT;
		s_device->CreateBuffer(&sbDesc, 0, particlesData.access());

		// Create the UAV for the structured buffer
		D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
		ZeroMemory(&sbUAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		sbUAVDesc.Buffer.FirstElement = 0;
		sbUAVDesc.Buffer.Flags = 0;
		sbUAVDesc.Buffer.NumElements = MAX_PARTICLE_NUM;
		sbUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		sbUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		s_device->CreateUnorderedAccessView(particlesData, &sbUAVDesc, particlesDataUAV.access());

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		shaderResourceViewDesc.Buffer.ElementOffset = 0;
		shaderResourceViewDesc.Buffer.ElementWidth = sizeof(XMVECTOR);
		shaderResourceViewDesc.Buffer.NumElements = MAX_PARTICLE_NUM;

		HRESULT result = s_device->CreateShaderResourceView(particlesData, &shaderResourceViewDesc, particlesDataSRV.access());

		ZeroMemory(&sbDesc, sizeof(D3D11_BUFFER_DESC));
		sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		sbDesc.CPUAccessFlags = 0;
		sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		sbDesc.StructureByteStride = sizeof(uint32_t);
		sbDesc.ByteWidth = sizeof(uint32_t) * 3;
		sbDesc.Usage = D3D11_USAGE_DEFAULT;
		s_device->CreateBuffer(&sbDesc, 0, particlesRange.access());

		ZeroMemory(&sbUAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		sbUAVDesc.Buffer.FirstElement = 0;
		sbUAVDesc.Buffer.Flags = 0;
		sbUAVDesc.Buffer.NumElements = 3;
		sbUAVDesc.Format = DXGI_FORMAT_R32_UINT;
		sbUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		s_device->CreateUnorderedAccessView(particlesRange, &sbUAVDesc, particlesRangeUAV.access());

		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.Format = DXGI_FORMAT_R32_UINT;
		shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		shaderResourceViewDesc.Buffer.ElementOffset = 0;
		shaderResourceViewDesc.Buffer.ElementWidth = sizeof(uint32_t);
		shaderResourceViewDesc.Buffer.NumElements = 3;

		s_device->CreateShaderResourceView(particlesRange, &shaderResourceViewDesc, particlesRangeSRV.access());
		/// <summary>
		ZeroMemory(&sbDesc, sizeof(D3D11_BUFFER_DESC));
		sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		sbDesc.CPUAccessFlags = 0;
		sbDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		sbDesc.StructureByteStride = sizeof(uint32_t);
		sbDesc.ByteWidth = sizeof(uint32_t) * 4;
		sbDesc.Usage = D3D11_USAGE_DEFAULT;
		s_device->CreateBuffer(&sbDesc, 0, indirectArgsBuffer.access());

		ZeroMemory(&sbUAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		sbUAVDesc.Buffer.FirstElement = 0;
		sbUAVDesc.Buffer.Flags = 0;
		sbUAVDesc.Buffer.NumElements = 4;
		sbUAVDesc.Format = DXGI_FORMAT_R32_UINT;
		sbUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		s_device->CreateUnorderedAccessView(indirectArgsBuffer, &sbUAVDesc, indirectArgsBufferUAV.access());
		/// </summary>

		std::vector<ShaderInfo> shadersParticleSpawner = { {ShaderType::VERTEX, L"shaders/disappearanceParticlesUpdater.hlsl", "VSMainSpawner"} };
		std::vector<ShaderInfo> shadersParticleUpdater = { {ShaderType::COMPUTE, L"shaders/disappearanceParticlesUpdater.hlsl", "CSMainUpdater"} };
		std::vector<ShaderInfo> shadersParticleRangeUpdater = { {ShaderType::COMPUTE, L"shaders/disappearanceParticlesUpdater.hlsl", "CSMainRange"} };
		std::vector<ShaderInfo> shadersParticlesDrawer = { {ShaderType::VERTEX, L"shaders/disappearanceParticles.hlsl", "VSMain"}, {ShaderType::PIXEL, L"shaders/disappearanceParticles.hlsl", "PSMain"} };


		std::vector<D3D11_INPUT_ELEMENT_DESC> iedDisappearance =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0,   DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"TANGENT", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ROWX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWY",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWZ",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWW",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TSCALE", 0, DXGI_FORMAT_R32_FLOAT,          1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TIMEPOINT", 0, DXGI_FORMAT_R32_FLOAT,       1, 68, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"SPHEREPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT,       1, 72, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"SPHEREGROWTHSPEED", 0, DXGI_FORMAT_R32_FLOAT,       1, 84, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTANCEID", 0, DXGI_FORMAT_R16_UINT,          1, 88, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		particleSpawner = ShaderManager::GetInstance()->getShader(shadersParticleSpawner, iedDisappearance);
		particleUpdater = ShaderManager::GetInstance()->getShader(shadersParticleUpdater);
		particleRangeUpdater = ShaderManager::GetInstance()->getShader(shadersParticleRangeUpdater);
		particlesDrawer = ShaderManager::GetInstance()->getShader(shadersParticlesDrawer);

		particleSpawner->createUniform(sizeof(XMMATRIX), ShaderType::VERTEX);
	}

	void ParticleSystem::addSmokeEmitter(const XMFLOAT3& pos, const XMFLOAT4& tint, float radius, float timePoint, std::shared_ptr<Texture> emission, std::shared_ptr<Texture> EMVA, std::shared_ptr<Texture> RLT, std::shared_ptr<Texture> BotBF)
	{
		SmokeEmitter emitter;
		emitter.init(pos, tint, radius, timePoint);
		emitter.emission = emission;
		emitter.EMVA = EMVA;
		emitter.RLT = RLT;
		emitter.BotBF = BotBF;
		m_smokeEmitters.push_back(emitter);
	}

	void ParticleSystem::setParticleTexture(std::shared_ptr<Texture> sparkTexture)
	{
		m_sparkTexture = sparkTexture;
	}

	void ParticleSystem::update(float timePoint)
	{
		for (auto& emitter : m_smokeEmitters)
			emitter.update(timePoint);
	}

	void ParticleSystem::render(const XMFLOAT3& cameraPos)
	{
		m_smokeShader->bind();

		particleInstanceBuffer.bind(0, sizeof(engine::SmokeParticle::Properties));	
		uint32_t particlesNum = 0;


		std::vector<SmokeParticle::Properties> particles;
		for (auto& emitter : m_smokeEmitters) 
		{
			for (auto& particle : emitter.getParticles())
				particles.push_back(particle.properties);
		}

		std::sort(particles.begin(), particles.end(),
			[&cameraPos](const SmokeParticle::Properties& a, const SmokeParticle::Properties& b) -> bool
			{
				return math::lengthVec3(a.pos - cameraPos) > math::lengthVec3(b.pos - cameraPos);
			});

		m_smokeEmitters[0].emission->bind(0);
		m_smokeEmitters[0].EMVA->bind(1);
		m_smokeEmitters[0].RLT->bind(2);
		m_smokeEmitters[0].BotBF->bind(3);

		auto mapping = particleInstanceBuffer.map();
		memcpy(mapping.pData, particles.data(), particles.size() * sizeof(SmokeParticle::Properties));
		

		particleInstanceBuffer.unmap();	
		s_devcon->DrawInstanced(6, particles.size(), 0, 0);
	}

	void ParticleSystem::renderDisappearanceParticles(Sky& sky, DepthStencil& depthStencil, DepthStencil& disabledDepthStencil, GBuffer& gbuffer, RenderTarget& renderTarget)
	{
		UINT initCounts = 0;
		
		ID3D11UnorderedAccessView* uavs[3] = { particlesDataUAV, particlesRangeUAV, indirectArgsBufferUAV };
		s_devcon->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 3, uavs, 0);

		disabledDepthStencil.bind();

		MeshSystem::GetInstance()->spawnDisappearanceParticles(particleSpawner);
		ID3D11UnorderedAccessView* uavs2[3] = { nullptr, nullptr, nullptr };
		s_devcon->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 3, uavs2, 0);
		
		particleUpdater->bind();

		s_devcon->CSSetUnorderedAccessViews(0, 1, particlesDataUAV.access(), &initCounts);
		s_devcon->CSSetUnorderedAccessViews(1, 1, particlesRangeUAV.access(), &initCounts);
		s_devcon->CSSetUnorderedAccessViews(2, 1, indirectArgsBufferUAV.access(), &initCounts);

		depthStencil.bindAsResourceCS(0);
		gbuffer.getNormalTarget().bindCS(1);
		uint32_t num = MAX_PARTICLE_NUM / 64 + 1;
		s_devcon->Dispatch(num, 1, 1);
		
		depthStencil.unbindAsResource(0);
		gbuffer.getNormalTarget().unbind(1);
		depthStencil.bind();
		particleRangeUpdater->bind();

		s_devcon->Dispatch(1, 1, 1);

		ID3D11UnorderedAccessView* uav[1] = { nullptr };
		s_devcon->CSSetUnorderedAccessViews(0, 1, uav, &initCounts);
		s_devcon->CSSetUnorderedAccessViews(1, 1, uav, &initCounts);
		s_devcon->CSSetUnorderedAccessViews(2, 1, uav, &initCounts);

		renderTarget.SetRenderTarget(depthStencil.getDepthStencilView());
		particlesDrawer->bind();

		s_devcon->VSSetShaderResources(0, 1, particlesDataSRV.access());
		s_devcon->VSSetShaderResources(1, 1, particlesRangeSRV.access());
		
		s_devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_sparkTexture->bind(2);

		s_devcon->DrawInstancedIndirect(indirectArgsBuffer, 0);

		ID3D11ShaderResourceView* v[1] = { nullptr };
		s_devcon->VSSetShaderResources(0, 1, v);
		s_devcon->VSSetShaderResources(1, 1, v);

	
	}

	void ParticleSystem::clean()
	{
		for (auto& emitter : s_manager->m_smokeEmitters)
			emitter.clean();

		s_manager->particleInstanceBuffer.clean();

	}
}