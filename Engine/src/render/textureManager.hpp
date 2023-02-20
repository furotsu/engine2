#pragma once

#include <memory>
#include <map>

#include "DxRes.hpp"
#include "texture.hpp"

namespace engine
{
	class TextureManager
	{
	protected:
		static TextureManager* s_manager;
		
		std::map<std::string, std::shared_ptr<Texture>> m_Textures;

	public:
		TextureManager();
		TextureManager(TextureManager& other) = delete;
		void operator=(const TextureManager&) = delete;

		static void init();
		static void deinit();
		static TextureManager* GetInstance();

		static std::shared_ptr<Texture> getTexture(std::string filepath, TextureType type = TextureType::TEXTURE_DEFAULT);


		static void clean();
	};
}