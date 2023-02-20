#include <algorithm>
#include <utility>

#include "modelManager.hpp"
#include "shaderManager.hpp"
#include "vegetationSystem.hpp"
#include "constants.hpp"

namespace engine
{
	VegetationSystem* VegetationSystem::s_manager = nullptr;

	VegetationSystem::VegetationSystem()
	{

		std::vector<ShaderInfo> shadersGrass = {
			{ShaderType::VERTEX, L"shaders/grass.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/grass.hlsl",  "PSMain"}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedGrass =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		m_grassShader = ShaderManager::GetInstance()->getShader(shadersGrass, iedGrass);

	}

	void VegetationSystem::init()
	{

		if (s_manager == nullptr)
		{
			s_manager = new VegetationSystem();
		}
		else
		{
			ASSERT(false && "Initializing \" TransformSystem \" singleton more than once ");
		}
	}

	void VegetationSystem::deinit()
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

	VegetationSystem* VegetationSystem::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ERROR("Trying to call \" TransformSystem \" singleton  instance before initializing it");
		}
#endif
		return s_manager;
	}

	void VegetationSystem::addGrassField(float size, const XMFLOAT3& position, std::shared_ptr<Texture> albedo, std::shared_ptr<Texture> normal, std::shared_ptr<Texture> opacity, std::shared_ptr<Texture> occlusion, float timePoint)
	{
		GrassField field;
		field.init(size, position, albedo, normal, opacity, occlusion, m_grassShader, timePoint);
		m_grassFields.push_back(field);
	}

	void VegetationSystem::render(float timePoint, const XMFLOAT3& cameraPos)
	{
		for (auto& field : m_grassFields)
			field.render(timePoint, cameraPos);
	}

	void VegetationSystem::clean()
	{
		for (auto& field : s_manager->m_grassFields)
			field.clean();
	}
}