#pragma once

#include "DirectXMath.h"
#include "DxRes.hpp"
#include "d3d.h"
#include "camera.hpp"
#include "window.hpp"
#include "postProcess.hpp"

namespace engine
{
	struct PerAppUniform
	{

	};

	struct PerFrameUniform
	{
		DirectX::XMMATRIX g_viewProj;
		DirectX::XMMATRIX g_view;
		DirectX::XMMATRIX g_viewInv;
		DirectX::XMMATRIX g_proj;
		DirectX::XMMATRIX g_projInv;
		DirectX::XMMATRIX g_lightProj;
		DirectX::XMFLOAT3 g_cameraPos;
		FLOAT g_screenWidth;
		FLOAT g_screenHeight;
		FLOAT g_EV100;
		FLOAT g_pointLightsNum;
		FLOAT g_reflectionMips;
		XMFLOAT2 g_nearFarPlanes;
		FLOAT g_pointlightShadowResolution;
		FLOAT g_timePoint;
		FLOAT g_frameDeltaTime;
		uint32_t g_maxParticles;
		uint32_t g_grassID;
		FLOAT dissolutionSpawnTime;
	};

	class Globals // a singletone for accessing global rendering resources
	{
	protected:
		static Globals* s_globals;

		DxResPtr<IDXGIFactory>		     m_factory;
		DxResPtr<IDXGIFactory5>			 m_factory5;
		DxResPtr<ID3D11Device>			 m_device;
		DxResPtr<ID3D11Device5>			 m_device5;
		DxResPtr<ID3D11DeviceContext>	 m_devcon;
		DxResPtr<ID3D11DeviceContext4>   m_devcon4;
		DxResPtr<ID3D11Debug>			 m_devdebug;

		DxResPtr<ID3D11Buffer> m_uniformGlobal;
		PerFrameUniform uniform;
		DxResPtr<ID3D11SamplerState> m_pSharedSampleState;
		DxResPtr<ID3D11SamplerState> m_pLinearClampSampleState;
		DxResPtr<ID3D11SamplerState> m_pShadowSampleState;
		DxResPtr<ID3D11SamplerState> m_pGrassSampleState;


		void initD3D();
		void initPerAppUniforms();
		void initGlobalUniforms();
		void initSharedSampleState();
		void setPerFrameUniforms(PerFrameUniform& data);
		void bindSharedSampleState();
	public:
		Globals();
		Globals(Globals& other) = delete;
		void operator=(const Globals&) = delete;

		static Globals* GetInstance();

		static void init();
		static void deinit();
		static void clean();

		void bind(const Window& window, const Camera& camera, PostProcess& postProcess, float timePoint, float frameFracTime);
	};
}