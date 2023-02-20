#pragma once

#define _USE_MATH_DEFINES

#include <vector>

#include "window.hpp"
#include "constants.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "model.hpp"
#include "utility.hpp"
#include "parallelExecutor.hpp"
#include "camera.hpp"
#include "sky.hpp"
#include "shaderManager.hpp"
#include "opaque_instances.hpp"
#include "renderTarget.hpp"
#include "gBuffer.hpp"
#include "postProcess.hpp"
#include "depthStencil.hpp"

using namespace math;

namespace engine
{
	constexpr uint16_t GRASS_STENCIL_VALUE = 2;
	constexpr uint16_t EMISSIVE_STENCIL_VALUE = 3;

	class Renderer
	{
	public:


		DepthStencil m_depthStencil;
		DepthStencil m_translucentDepthStencil;
		DepthStencil m_disabledDepthStencil;
		DepthStencil m_deferredDepthStencil;
		DxResPtr<ID3D11BlendState1> m_pBlendStateTransparent;
		DxResPtr<ID3D11BlendState1> m_pBlendStateParticles;
		DxResPtr<ID3D11RasterizerState> m_pOpaqueRasterizer;
		DxResPtr<ID3D11RasterizerState> m_pDecalRasterizer;
		DxResPtr<ID3D11RasterizerState> m_pTransparentRasterizer;
		DxResPtr<ID3D11DepthStencilState> m_grassStencilRenderState;
		DxResPtr<ID3D11DepthStencilState> m_grassStencilShadingState;
		std::shared_ptr<ShaderProgram> m_depthMinSample;
		std::shared_ptr<ShaderProgram> m_deferredShader;
		std::shared_ptr<ShaderProgram> m_deferredGrassShader;
		std::shared_ptr<ShaderProgram> m_deferredEmissiveShader;
		uint32_t m_shadowMapId;

		RenderTarget m_mainRenderTarget;
		RenderTarget m_renderTarget;
		RenderTarget m_copiedNormalTexture;

		GBuffer m_gBuffer;


	public:

		Sky skybox;

		Renderer() = default;

		void init(const Window& window, uint32_t pointLightNum);

		void setSkybox(Sky skybox, std::vector<ShaderInfo> shaders, std::string irradianceFile, std::string reflectionFile, std::string reflectanceFile);

		void renderFrame(const Camera& camera, Window& window, const PostProcess& postProcess, float timePoint);

		void renderOpaque(const Camera& camera, Window& window, float timePoint);
		void deferredRender();
		void renderTranslucent(const Camera& camera);

		void onResize(const Window& window);
		
		void clean();

	protected:

	};


}