#include <math.h>
#include <algorithm>
#include <cmath>

#include "d3d.hpp"
#include "renderer.hpp"
#include "debug.hpp"
#include "meshSystem.hpp"
#include "shadowSystem.hpp"
#include "vegetationSystem.hpp"
#include "decalSystem.hpp"
#include "particleSystem.hpp"

namespace engine
{
	void Renderer::init(const Window& window, uint32_t pointLightNum)
	{
		std::vector<ShaderInfo> shadersDepthMinSample = {
			{ShaderType::VERTEX, L"shaders/depthMinSample.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/depthMinSample.hlsl",  "PSMain"}
		};

		std::vector<ShaderInfo> shadersDeferred = {
			{ShaderType::VERTEX, L"shaders/deferred.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/deferred.hlsl",  "PSMain"}
		};

		std::vector<ShaderInfo> shadersDeferredGrass = {
			{ShaderType::VERTEX, L"shaders/deferredGrass.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/deferredGrass.hlsl",  "PSMain"}
		};

		std::vector<ShaderInfo> shadersDeferredEmissive = {
			{ShaderType::VERTEX, L"shaders/deferredEmissive.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/deferredEmissive.hlsl",  "PSMain"}
		};

		m_depthMinSample = ShaderManager::GetInstance()->getShader(shadersDepthMinSample);
		m_deferredShader = ShaderManager::GetInstance()->getShader(shadersDeferred);
		m_deferredGrassShader = ShaderManager::GetInstance()->getShader(shadersDeferredGrass);
		m_deferredEmissiveShader = ShaderManager::GetInstance()->getShader(shadersDeferredEmissive);
		
		ALWAYS_ASSERT(m_mainRenderTarget.init(window.m_width, window.m_height, DXGI_FORMAT_R16G16B16A16_FLOAT, MULTISAMPLES_COUNT));
		ALWAYS_ASSERT(m_renderTarget.init(window.m_width, window.m_height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1));
		ALWAYS_ASSERT(m_copiedNormalTexture.init(window.m_width, window.m_height, DXGI_FORMAT_R16G16_SNORM, MULTISAMPLES_COUNT));

		m_gBuffer.init(window);
		DecalSystem::GetInstance()->initObjTextureCopy(window);
		ALWAYS_ASSERT(m_disabledDepthStencil.init(window.m_width, window.m_height, engine::DepthStencil::Type::Disabled));
		ALWAYS_ASSERT(m_deferredDepthStencil.init(window.m_width, window.m_height, engine::DepthStencil::Type::Deferred));
		ALWAYS_ASSERT(m_depthStencil.init(window.m_width, window.m_height, DepthStencil::Type::DepthStencil));

		m_shadowMapId = ShadowSystem::GetInstance()->createShadowMap(pointLightNum);

		D3D11_BLEND_DESC1 BlendState;
		ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC1));
		BlendState.RenderTarget[0].BlendEnable = TRUE;
		BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		s_device->CreateBlendState1(&BlendState, m_pBlendStateParticles.access());

		BlendState.AlphaToCoverageEnable = TRUE;
		s_device->CreateBlendState1(&BlendState, m_pBlendStateTransparent.access());

		D3D11_RASTERIZER_DESC wfdesc;
		ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
		wfdesc.FillMode = D3D11_FILL_SOLID;
		wfdesc.CullMode = D3D11_CULL_BACK;
		s_device->CreateRasterizerState(&wfdesc, m_pOpaqueRasterizer.access());

		ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
		wfdesc.FillMode = D3D11_FILL_SOLID;
		wfdesc.CullMode = D3D11_CULL_NONE;
		s_device->CreateRasterizerState(&wfdesc, m_pTransparentRasterizer.access());

		ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
		wfdesc.FillMode = D3D11_FILL_SOLID;
		wfdesc.CullMode = D3D11_CULL_FRONT;
		s_device->CreateRasterizerState(&wfdesc, m_pDecalRasterizer.access());

		D3D11_DEPTH_STENCIL_DESC dsDesc;
		//Create depth-stencil state

		// Depth test parameters
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

		// Stencil test parameters
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		HRESULT hr = s_device->CreateDepthStencilState(&dsDesc, m_grassStencilRenderState.access());

		// Depth test parameters
		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

		// Stencil test parameters
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		// Create depth stencil state
		hr = s_device->CreateDepthStencilState(&dsDesc, m_grassStencilShadingState.access());
	}

	void Renderer::setSkybox(Sky skybox, std::vector<ShaderInfo> shaders, std::string irradianceFile, std::string reflectionFile, std::string reflectanceFile)
	{
		this->skybox = skybox;
		this->skybox.init(shaders, irradianceFile, reflectionFile, reflectanceFile);
	}

	void Renderer::renderFrame(const Camera& camera, Window& window, const PostProcess& postProcess, float timePoint)
	{
		m_gBuffer.clearRenderTargets();
		renderOpaque(camera, window, timePoint);

		deferredRender();
		
		m_disabledDepthStencil.bindAsResourcePS(0);

		m_depthStencil.bind();
		m_mainRenderTarget.SetRenderTarget(m_depthStencil.getDepthStencilView());
		skybox.render(camera);

		renderTranslucent(camera);

		window.bindTarget(m_disabledDepthStencil.getDepthStencilView());
		m_disabledDepthStencil.bind();

		postProcess.resolve(m_mainRenderTarget);
		window.present();
	}

	void Renderer::renderOpaque(const Camera& camera, Window& window, float timePoint)
	{
		s_devcon->RSSetState(m_pOpaqueRasterizer);

		window.setTmpViewPort(SHADOWMAP_SIDE_SIZE, SHADOWMAP_SIDE_SIZE);
		ShadowSystem::GetInstance()->unbindMapAsResource(4);
		ShadowSystem::GetInstance()->unbindMapAsResource(6);
		ShadowSystem::GetInstance()->renderToShadowMap(m_shadowMapId, camera);
		m_mainRenderTarget.ClearRenderTarget();
		window.setViewPort();

		m_gBuffer.bind(m_depthStencil.getDepthStencilView());

		m_depthStencil.bind();
		m_depthStencil.clearView();

		MeshSystem::GetInstance()->render(skybox);

		s_devcon->OMSetDepthStencilState(m_grassStencilRenderState, EMISSIVE_STENCIL_VALUE);
		MeshSystem::GetInstance()->renderEmissive();


		//transparent objects
		m_depthStencil.bind();
		s_devcon->RSSetState(m_pTransparentRasterizer);
		MeshSystem::GetInstance()->renderDissolution(skybox);
		MeshSystem::GetInstance()->renderDisappearance(skybox);

		m_gBuffer.bind(m_depthStencil.getDepthStencilView());

		s_devcon->OMSetDepthStencilState(m_grassStencilRenderState, GRASS_STENCIL_VALUE);
		ShadowSystem::GetInstance()->bindMapAsResource(m_shadowMapId, 4);
		VegetationSystem::GetInstance()->render(timePoint, camera.positionVec3());
	}

	void Renderer::deferredRender()
	{
		skybox.bindIBLTextures(7);

		s_devcon->RSSetState(m_pDecalRasterizer);

		s_devcon->CopyResource(m_copiedNormalTexture.getRenderTargetRes(), m_gBuffer.getNormalTarget().getRenderTargetRes());
		
		m_gBuffer.bind(m_disabledDepthStencil.getDepthStencilView());
		m_disabledDepthStencil.bind();
		m_depthStencil.bindAsResourcePS(0);

		DecalSystem::GetInstance()->render(m_gBuffer.getObjIDTarget(), m_copiedNormalTexture);
		
		s_devcon->CopyResource(m_deferredDepthStencil.getRenderTargetRes(), m_depthStencil.getRenderTargetRes());
		
		m_mainRenderTarget.SetRenderTarget(m_deferredDepthStencil.getDepthStencilView());

		m_deferredShader->bind();
		m_gBuffer.bindAsTextures(1);

		ShadowSystem::GetInstance()->bindMapAsResource(m_shadowMapId, 6);

		s_devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		s_devcon->RSSetState(m_pOpaqueRasterizer);
		
		m_deferredDepthStencil.bind();
		s_devcon->Draw(3, 0);

		m_deferredGrassShader->bind();
		s_devcon->OMSetDepthStencilState(m_grassStencilShadingState, GRASS_STENCIL_VALUE);
		s_devcon->Draw(3, 0);

		m_deferredEmissiveShader->bind();
		s_devcon->OMSetDepthStencilState(m_grassStencilShadingState, EMISSIVE_STENCIL_VALUE);
		s_devcon->Draw(3, 0);

		m_gBuffer.unbindAsTextures(1);
	}

	void Renderer::renderTranslucent(const Camera& camera)
	{
		//transparent objects
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		UINT sampleMask = 0xffffffff;

		s_devcon->OMSetBlendState(m_pBlendStateParticles, blendFactor, sampleMask);

		m_disabledDepthStencil.bindAsResourcePS(0);

		m_mainRenderTarget.SetRenderTarget(m_disabledDepthStencil.getDepthStencilView());

		m_renderTarget.bindPS(5);
		ShadowSystem::GetInstance()->bindMapAsResource(m_shadowMapId, 4);

		ParticleSystem::GetInstance()->renderDisappearanceParticles(skybox, m_depthStencil, m_disabledDepthStencil, m_gBuffer, m_mainRenderTarget);
		m_disabledDepthStencil.bindAsResourcePS(0);

		m_mainRenderTarget.SetRenderTarget(m_disabledDepthStencil.getDepthStencilView());
		m_depthStencil.bindAsResourcePS(5);
		ParticleSystem::GetInstance()->render(camera.positionVec3());

		m_renderTarget.unbind(5);

		s_devcon->OMSetBlendState(0, 0, sampleMask);
	}

	void Renderer::onResize(const Window& window)
	{
		m_depthStencil.clean();
		m_deferredDepthStencil.clean();
		m_mainRenderTarget.clean();
		m_copiedNormalTexture.clean();
		m_renderTarget.clean();
		m_gBuffer.clean();

		if (window.hWnd)
		{
			m_depthStencil.init(window.m_width, window.m_height, DepthStencil::Type::DepthStencil);
			m_deferredDepthStencil.init(window.m_width, window.m_height, DepthStencil::Type::Deferred);
			m_deferredDepthStencil.clearView();
		}

		if (window.m_width != 0 || window.m_height != 0)
		{
			m_mainRenderTarget.init(window.m_width, window.m_height, DXGI_FORMAT_R16G16B16A16_FLOAT, MULTISAMPLES_COUNT);
			m_renderTarget.init(window.m_width, window.m_height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
			m_copiedNormalTexture.init(window.m_width, window.m_height, DXGI_FORMAT_R16G16_SNORM, MULTISAMPLES_COUNT);
			m_gBuffer.init(window);
			DecalSystem::GetInstance()->initObjTextureCopy(window);
		}
	}

	void Renderer::clean()
	{
		skybox.clean();

		m_deferredDepthStencil.clean();
		m_disabledDepthStencil.clean();
		m_depthStencil.clean();
		m_mainRenderTarget.clean();
		m_renderTarget.clean();

		m_pBlendStateParticles.release();
		m_pBlendStateTransparent.release();

		m_pOpaqueRasterizer.release();
		m_pTransparentRasterizer.release();
	}
}