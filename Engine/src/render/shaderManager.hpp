#pragma once

#include <memory>
#include <map>

#include "DxRes.hpp"
#include "shader.hpp"

namespace engine
{
	class ShaderManager
	{
	protected:
		static ShaderManager* s_manager;

		std::map<const wchar_t*, std::shared_ptr<ShaderProgram>> m_shaders;

	public:
		ShaderManager();
		ShaderManager(ShaderManager& other) = delete;
		void operator=(const ShaderManager&) = delete;

		static void init();
		static void deinit();
		static ShaderManager* GetInstance();

		std::shared_ptr<ShaderProgram> getShader(std::vector<ShaderInfo> shaders, std::vector<D3D11_INPUT_ELEMENT_DESC> ied);
		std::shared_ptr<ShaderProgram> getShader(std::vector<ShaderInfo> shaders);

		static void clean();
	};
}