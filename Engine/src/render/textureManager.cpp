#include "textureManager.hpp"
#include "debug.hpp"

namespace engine
{
	TextureManager* TextureManager::s_manager = nullptr;

	TextureManager::TextureManager()
	{
	}

	void TextureManager::init()
	{
		if (s_manager == nullptr)
		{
			s_manager = new TextureManager();
		}
		else
		{
			ASSERT(false && "Initializing \" TextureManager \" singleton more than once ");
		}
	}

	void TextureManager::deinit()
	{
		if (s_manager == nullptr)
		{
			ASSERT(false && "Trying to delete \" TextureManager \" singleton more than once ");
		}
		else
		{
			clean();
			delete s_manager;
			s_manager = nullptr;
		}
	}

	TextureManager* TextureManager::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ERROR("Trying to call \" TextureManager \" singleton  instance before initializing it");
		}
#endif
		return s_manager;
	}

	std::shared_ptr<Texture> TextureManager::getTexture(std::string filepath, TextureType type)
	{
		auto it = GetInstance()->m_Textures.find(filepath);
		
		if (it != GetInstance()->m_Textures.end())
		{
			return it->second;

		}
		else
		{
			Texture tex;
			tex.init(filepath, type);
			std::shared_ptr<Texture> res = std::make_shared<Texture>(tex);
			GetInstance()->m_Textures.emplace(std::make_pair(filepath, res));
			return res;
		}
		return nullptr;
	}

	void TextureManager::clean()
	{
		for (auto& elem : GetInstance()->m_Textures)
		{
			elem.second->clean();
		}
	}
}