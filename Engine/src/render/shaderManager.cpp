#include "shaderManager.hpp"

namespace engine
{
	ShaderManager* ShaderManager::s_manager = nullptr;

	ShaderManager::ShaderManager()
	{
	}

	void ShaderManager::init()
	{
		if (s_manager == nullptr)
		{
			s_manager = new ShaderManager();
		}
		else
		{
			ASSERT(false && "Initializing \" ShaderManager \" singleton more than once ");
		}
	}

	void ShaderManager::deinit()
	{
		if (s_manager == nullptr)
		{
			ERROR(false && "Trying to delete \" ShaderManager \" singleton more than once ");
		}
		else
		{
			GetInstance()->clean();
			delete s_manager;
			s_manager = nullptr;
		}
	}

	ShaderManager* ShaderManager::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ERROR("Trying to call \" ShaderManager \" singleton  instance before initializing it");
		}
#endif

		return s_manager;
	}

	std::shared_ptr<ShaderProgram> ShaderManager::getShader(std::vector<ShaderInfo> shaders, std::vector<D3D11_INPUT_ELEMENT_DESC> ied)
	{
		ShaderProgram shader;
		shader.init(shaders, ied);
		std::shared_ptr<ShaderProgram> res = std::make_shared<ShaderProgram>(shader);
		m_shaders.emplace(std::make_pair(shaders[0].filePath, res));
		return res;
	}

	std::shared_ptr<ShaderProgram> ShaderManager::getShader(std::vector<ShaderInfo> shaders)
	{
		ShaderProgram shader;
		shader.init(shaders);
		std::shared_ptr<ShaderProgram> res = std::make_shared<ShaderProgram>(shader);
		m_shaders.emplace(std::make_pair(shaders[0].filePath, res));
		return res;
	}


	void ShaderManager::clean()
	{
		for (auto& elem : GetInstance()->m_shaders)
		{
			elem.second->clean();
		}
	}
}