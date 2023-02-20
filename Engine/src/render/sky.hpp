#pragma once

#include "d3d.h"
#include "DXRes.hpp"
#include "shader.hpp"
#include "textureManager.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "shaderManager.hpp"


namespace engine
{
	class Sky
	{
		std::shared_ptr<ShaderProgram> m_skyboxProgram;
		std::shared_ptr<Texture> m_texture;

	public:
		std::shared_ptr<Texture> irradianceMap;
		std::shared_ptr<Texture> reflectionMap;
		std::shared_ptr<Texture> reflectanceMap;


		Sky() = default;
		Sky& operator=(const Sky& other)
		{
			m_texture = other.m_texture;
			return *this;
		}

		Sky(std::string filepath);

		void init(std::vector<ShaderInfo>& shaders, std::string irradianceFile, std::string reflectionFile, std::string reflectanceFile);

		void bindIBLTextures(uint16_t startSlot);
		void clean();

		void render(const Camera& camera);

		void bind();
	};
}