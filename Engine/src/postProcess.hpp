#pragma once

#include "renderTarget.hpp"
#include "shaderManager.hpp"

namespace engine
{
	class PostProcess
	{
		float EV100;
		std::shared_ptr<ShaderProgram> m_postRenderProgram;

	public:
		PostProcess() = default;
		PostProcess(float EV100, std::vector<ShaderInfo>& shaders)
			: EV100(EV100)
		{
			m_postRenderProgram = ShaderManager::GetInstance()->getShader(shaders);
		}

		inline void changeEV100(float value) { EV100 += value; }
		float getEV100() { return EV100; }

		void resolve(RenderTarget& MSAAInput) const;
		void clean();
	};
}