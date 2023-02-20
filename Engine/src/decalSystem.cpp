#include <algorithm>
#include <utility>

#include "modelManager.hpp"
#include "shaderManager.hpp"
#include "decalSystem.hpp"
#include "constants.hpp"
#include "transformSystem.hpp"
#include "random.hpp"
#include "globals.hpp"

namespace engine
{
	DecalSystem* DecalSystem::s_manager = nullptr;

	DecalSystem::DecalSystem()
	{

		std::vector<ShaderInfo> shadersGrass = {
			{ShaderType::VERTEX, L"shaders/decal.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/decal.hlsl",  "PSMain"}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedDecal =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ROWX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWY",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWZ",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWW",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INVROWX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INVROWY",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INVROWZ",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INVROWW",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ALBEDO", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 128, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROUGHNESS", 0, DXGI_FORMAT_R32_FLOAT, 1, 140, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"METALNESS", 0, DXGI_FORMAT_R32_FLOAT, 1, 144, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"OBJID", 0, DXGI_FORMAT_R16_UINT, 1, 148, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		m_decalShader = ShaderManager::GetInstance()->getShader(shadersGrass, iedDecal);
		pixelBufferID = m_decalShader->createUniform(sizeof(XMMATRIX), ShaderType::PIXEL);
		m_cubeModel = ModelManager::GetInstance()->initUnitCube();
	}

	void DecalSystem::init()
	{

		if (s_manager == nullptr)
		{
			s_manager = new DecalSystem();
		}
		else
		{
			ASSERT(false && "Initializing \" TransformSystem \" singleton more than once ");
		}
	}

	void DecalSystem::deinit()
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

	DecalSystem* DecalSystem::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ERROR("Trying to call \" TransformSystem \" singleton  instance before initializing it");
		}
#endif
		return s_manager;
	}

	void DecalSystem::initObjTextureCopy(const Window& window)
	{
		m_objTextureCopy.clean();
		m_objTextureCopy.init(window.m_width, window.m_height, DXGI_FORMAT_R16_UINT, MULTISAMPLES_COUNT);
	}

	void DecalSystem::addDecal(const XMMATRIX& modelTransform, uint32_t parentTransformId, uint16_t instanceID)
	{
		Decal decal;
		decal.transformID = TransformSystem::GetInstance()->addTransform(modelTransform, parentTransformId);
		decal.properties.transform = modelTransform;

		decal.properties.albedo = XMFLOAT3(math::randRangef(0.0f, 1.0f), math::randRangef(0.0f, 1.0f), math::randRangef(0.0f, 1.0f));
		decal.properties.metalness = 0.001f;
		decal.properties.roughness = 0.99f;
		decal.properties.objID = instanceID;
		m_decals.push_back(decal);
		updateInstanceBuffers();
	}

	void DecalSystem::setDefaultNormalTexture(std::shared_ptr<Texture> NormalTexture)
	{
		m_defaultNormalTexture = NormalTexture;
	}

	void DecalSystem::updateInstanceBuffers()
	{
		uint32_t totalInstances = m_decals.size();

		if (totalInstances == 0)
			return;

		instanceBuffer.init(totalInstances * sizeof(Decal::Properties), D3D11_USAGE_DYNAMIC); // resizes if needed

		auto mapping = instanceBuffer.map();
		Decal::Properties* dst = static_cast<Decal::Properties*>(mapping.pData);

		uint32_t copiedNum = 0;
		for (const auto& decal : m_decals)
		{
			Decal::Properties prop;
			prop = decal.properties;
			prop.transform = TransformSystem::GetInstance()->getTransform(decal.transformID);
			prop.inverseTransform = TransformSystem::GetInstance()->getTransformInverse(decal.transformID);
			dst[copiedNum++] = prop;
		}

		instanceBuffer.unmap();
	}

	void DecalSystem::render(RenderTarget& objIDTarget, RenderTarget& normalTextureTarget)
	{
		if (instanceBuffer.size() == 0)
			return;

		m_decalShader->bind();
		instanceBuffer.bind(1, sizeof(Decal::Properties));

		uint32_t renderedInstances = 0;
		s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_cubeModel->vertexBuffer().bind(0, sizeof(Mesh::Vertex));

		s_devcon->CopyResource(m_objTextureCopy.getRenderTargetRes(), objIDTarget.getRenderTargetRes());


		normalTextureTarget.bindPS(1);
		m_objTextureCopy.bindPS(2);
		m_defaultNormalTexture->bind(3);

		const auto& meshRange = m_cubeModel->meshRange(0);

		s_devcon->DrawInstanced(meshRange.vertexNum, m_decals.size(), meshRange.vertexOffset, renderedInstances);
	}


	void DecalSystem::clean()
	{
		s_manager->m_cubeModel = nullptr;
		s_manager->m_decalShader = nullptr;
		s_manager->m_defaultNormalTexture = nullptr;
		s_manager->instanceBuffer.clean();
	}
}