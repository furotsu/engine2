#include "win.hpp"
#include "globals.hpp"
#include "debug.hpp"
#include "lightSystem.hpp"
#include "hlsl.hpp"
// Say NVidia or AMD driver to prefer a dedicated GPU instead of an integrated.
// This has effect on laptops.
extern "C"
{
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

namespace engine
{
	Globals* Globals::s_globals = nullptr;

	Globals::Globals()
	{
		initD3D();
		initGlobalUniforms();
		initSharedSampleState();
		uniform.g_lightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), 1.0f, CAMERA_FAR_PLANE, CAMERA_NEAR_PLANE );
	}

	void Globals::init()
	{
		if (s_globals == nullptr)
		{
			s_globals = new Globals();
		}
		else
		{
			ASSERT(false && "Initializing \" Globals \" singleton more than once ");
		}
	}

	void Globals::deinit()
	{
		if (s_globals == nullptr)
		{
			ASSERT(false && "Trying to delete \" Globals \" singleton more than once ");
		}
		else
		{
			clean();
			delete s_globals;
			s_globals = nullptr;
		}
	}

	Globals* Globals::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_globals == nullptr)
		{
			ASSERT( false && "Trying to call \" Globals \" singleton  instance before initializing it");
		}
#endif 
		return s_globals;
	}

	void Globals::initD3D()
	{
		HRESULT result;

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)m_factory.reset());
		ASSERT(result >= 0 && "CreateDXGIFactory");

		result = m_factory->QueryInterface(__uuidof(IDXGIFactory5), (void**)m_factory5.reset());
		ALWAYS_ASSERT(result >= 0 && "Query IDXGIFactory5");

		{
			uint32_t index = 0;
			IDXGIAdapter1* adapter;
			while (m_factory5->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);
				std::wstring wsrt(desc.Description);
				LOG("Globals.cpp", "GPU # " + std::to_string(index) + " " + std::string(wsrt.begin(), wsrt.end()));
			}
		}

		// Init D3D Device & Context
		const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_12_0;
		D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_12_0;
		result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
			&featureLevelRequested, 1, D3D11_SDK_VERSION, m_device.reset(), &featureLevelInitialized, m_devcon.reset());
		ALWAYS_ASSERT(result >= 0 && "D3D11CreateDevice");
		ALWAYS_ASSERT(featureLevelRequested == featureLevelInitialized && "D3D_FEATURE_LEVEL_11_0");

		result = m_device->QueryInterface(__uuidof(ID3D11Device5), (void**)m_device5.reset());
		ALWAYS_ASSERT(result >= 0 && "Query ID3D11Device5");

		result = m_devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)m_devcon4.reset());
		ALWAYS_ASSERT(result >= 0 && "Query ID3D11DeviceContext4");

		result = m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)m_devdebug.reset());
		ALWAYS_ASSERT(result >= 0 && "Query ID3D11Debug");

		// Write global pointers

		s_factory = m_factory5.ptr();
		s_device = m_device5.ptr();
		s_devcon = m_devcon4.ptr();
	}

	void Globals::initPerAppUniforms()
	{
		//Create global uniform buffer
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

		cbbd.Usage = D3D11_USAGE_DYNAMIC;
		cbbd.ByteWidth = sizeof(PerAppUniform);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbbd.MiscFlags = 0;

		HRESULT hr = s_device->CreateBuffer(&cbbd, NULL, m_uniformGlobal.reset());
		ASSERT(hr >= 0 && " cannot create perApp uniform buffer");

		PerAppUniform u;

		D3D11_MAPPED_SUBRESOURCE res;

		s_devcon->Map(m_uniformGlobal, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &res);
		memcpy(res.pData, &u, sizeof(PerAppUniform));
		s_devcon->Unmap(m_uniformGlobal, NULL);

		s_devcon->VSSetConstantBuffers(PER_FRAME_UNIFORM_SLOT, 1, m_uniformGlobal.access());
		s_devcon->GSSetConstantBuffers(PER_FRAME_UNIFORM_SLOT, 1, m_uniformGlobal.access());
		s_devcon->PSSetConstantBuffers(PER_FRAME_UNIFORM_SLOT, 1, m_uniformGlobal.access());
		s_devcon->CSSetConstantBuffers(PER_FRAME_UNIFORM_SLOT, 1, m_uniformGlobal.access());
	}

	void Globals::initGlobalUniforms()
	{
		//Create global uniform buffer
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

		cbbd.Usage = D3D11_USAGE_DYNAMIC;
		cbbd.ByteWidth = sizeof(PerFrameUniform);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbbd.MiscFlags = 0;

		HRESULT hr = s_device->CreateBuffer(&cbbd, NULL, m_uniformGlobal.reset());
		ASSERT(hr >= 0 && " cannot create global uniform buffer");
	}

	void Globals::initSharedSampleState()
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = s_device->CreateSamplerState(&sampDesc, m_pSharedSampleState.reset());
		ASSERT(hr >= 0 && L"unable to create shared sampler state");

		sampDesc.Filter =	D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_GREATER;

		hr = s_device->CreateSamplerState(&sampDesc, m_pShadowSampleState.reset());
		ASSERT(hr >= 0 && L"unable to create shadow sampler state");

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		hr = s_device->CreateSamplerState(&sampDesc, m_pLinearClampSampleState.reset());
		ASSERT(hr >= 0 && L"unable to create shadow sampler state");

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MaxLOD = 4.0f;

		hr = s_device->CreateSamplerState(&sampDesc, m_pGrassSampleState.reset());
		ASSERT(hr >= 0 && L"unable to create shadow sampler state");
	}

	void Globals::clean()
	{
		s_globals->m_uniformGlobal.release();
		s_globals->m_pSharedSampleState.release();
		s_globals->m_pLinearClampSampleState.release();
		s_globals->m_pShadowSampleState.release();
		s_globals->m_pGrassSampleState.release();

		s_globals->m_factory.release();
		s_globals->m_factory5.release();
		s_globals->m_device.release();
		s_globals->m_device5.release();
		s_globals->m_devcon.release();
		s_globals->m_devcon4.release();
		s_globals->m_devdebug.release();
	}

	void Globals::bind(const Window& window, const Camera& camera, PostProcess& postProcess, float timePoint, float frameFracTime)
	{
		LightSystem::GetInstance()->bindLights();
		uniform.g_viewProj = camera.getViewProj();
		uniform.g_view = camera.getView();
		uniform.g_viewInv = camera.getViewInv();
		uniform.g_proj = camera.getProj();
		uniform.g_projInv = camera.getProjInv();
		uniform.g_cameraPos = XMFLOAT3(XMVectorGetX(camera.position()), XMVectorGetY(camera.position()), XMVectorGetZ(camera.position()));
		uniform.g_screenWidth = window.m_width;
		uniform.g_screenHeight = window.m_height;
		uniform.g_pointLightsNum = LightSystem::GetInstance()->getPointLightsNum();
		uniform.g_EV100 = postProcess.getEV100();
		uniform.g_reflectionMips = REFLECTION_MIPS_COUNT;
		uniform.g_nearFarPlanes = XMFLOAT2(CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE);
		uniform.g_pointlightShadowResolution = SHADOWMAP_SIDE_SIZE;
		uniform.g_timePoint = timePoint;
		uniform.g_frameDeltaTime = frameFracTime;
		uniform.g_maxParticles = MAX_PARTICLE_NUM;
		uniform.g_grassID = GRASS_ID;
		uniform.dissolutionSpawnTime = DISSOLUTION_SPAWNTIME;

		setPerFrameUniforms(uniform);
		bindSharedSampleState();
	}

	void Globals::setPerFrameUniforms(PerFrameUniform& data)
	{
		D3D11_MAPPED_SUBRESOURCE res;

		s_devcon->Map(m_uniformGlobal, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &res);
		memcpy(res.pData, &data, sizeof(PerFrameUniform));
		s_devcon->Unmap(m_uniformGlobal, NULL);

		s_devcon->VSSetConstantBuffers(0, 1, m_uniformGlobal.access()); // 0 since we dont use perApp buffer
		s_devcon->GSSetConstantBuffers(0, 1, m_uniformGlobal.access());
		s_devcon->PSSetConstantBuffers(0, 1, m_uniformGlobal.access());
		s_devcon->CSSetConstantBuffers(0, 1, m_uniformGlobal.access());
	}

	void Globals::bindSharedSampleState()
	{
		s_devcon->PSSetSamplers(0, 1, m_pSharedSampleState.access());
		s_devcon->CSSetSamplers(0, 1, m_pSharedSampleState.access());
		s_devcon->PSSetSamplers(1, 1, m_pShadowSampleState.access());
		s_devcon->PSSetSamplers(2, 1, m_pLinearClampSampleState.access());
		s_devcon->PSSetSamplers(3, 1, m_pGrassSampleState.access());
	}
}