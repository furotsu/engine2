#include <vector>

#include "depthStencil.hpp"
#include "debug.hpp"
#include "constants.hpp"

bool engine::DepthStencil::init(uint32_t width, uint32_t height, DepthStencil::Type type, uint32_t numCubes)
{
	m_type = type;
	switch (type)
	{
	case engine::DepthStencil::Type::DepthStencil:
	{
		return initDefaultDepthStencil(width, height);
	}break;
	case engine::DepthStencil::Type::Cubemap:
	{
		return initCubemapDepthStencil(width, height, numCubes);
	}break;
	case engine::DepthStencil::Type::ShadowDepth:
	{
		return initShadowDepthStencil(width, height);
	}break;
	case engine::DepthStencil::Type::Deferred:
	{
		return initDeferredDepthStencil(width, height);
	}break;
	case engine::DepthStencil::Type::Disabled:
	{
		return initDisabledDepthStencil();
	}
	default: return false; break;
	}
}

bool engine::DepthStencil::initDefaultDepthStencil(uint32_t width, uint32_t height)
{
	//Create depth-stencil buffer using texture resource
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.SampleDesc.Count = MULTISAMPLES_COUNT;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = NULL;
	descDepth.MiscFlags = NULL;

	HRESULT hr = s_device->CreateTexture2D(&descDepth, NULL, m_pDepthStencil.reset());
	ASSERT(hr >= 0 && " cannot create depth-stencil uniform buffer");
	if (FAILED(hr))
		return false;
	//Create depth-stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc;

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
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	hr = s_device->CreateDepthStencilState(&dsDesc, m_pDSState.reset());
	if (FAILED(hr))
		return false;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.Flags = NULL;
	descDSV.ViewDimension = (MULTISAMPLES_COUNT == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;

	hr = s_device->CreateDepthStencilView(m_pDepthStencil, &descDSV, m_pDSV.reset());
	ASSERT(hr >= 0 && " cannot create depth-stencil view");


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = (MULTISAMPLES_COUNT > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	hr = s_device->CreateShaderResourceView(m_pDepthStencil, &shaderResourceViewDesc, m_pSRV.access());

	if (FAILED(hr))
		return false;
	return true;
}

bool engine::DepthStencil::initShadowDepthStencil(uint32_t width, uint32_t height)
{
	//Create depth-stencil buffer using texture resource
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.SampleDesc.Count = MULTISAMPLES_COUNT;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.CPUAccessFlags = NULL;
	descDepth.MiscFlags = NULL;

	HRESULT hr = s_device->CreateTexture2D(&descDepth, NULL, m_pDepthStencil.reset());
	ASSERT(hr >= 0 && " cannot create depth-stencil uniform buffer");
	if (FAILED(hr))
		return false;
	//Create depth-stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc;

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
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	hr = s_device->CreateDepthStencilState(&dsDesc, m_pDSState.reset());
	if (FAILED(hr))
		return false;
	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = (MULTISAMPLES_COUNT == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = NULL;

	hr = s_device->CreateDepthStencilView(m_pDepthStencil, &descDSV, m_pDSV.reset());
	ASSERT(hr >= 0 && " cannot create depth-stencil view");
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = (MULTISAMPLES_COUNT == 1) ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = s_device->CreateShaderResourceView(m_pDepthStencil, &srvDesc, m_pSRV.reset());
	ASSERT(hr >= 0 && " cannot create shader resource view");
	if (FAILED(hr))
		return false;

	return true;
}

bool engine::DepthStencil::initCubemapDepthStencil(uint32_t width, uint32_t height, uint32_t numCubes)
{
	//Create depth-stencil buffer using texture resource
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Width = width;
	shadowMapDesc.Height = height;
	shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowMapDesc.ArraySize = 6 * numCubes;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.CPUAccessFlags = 0;
	shadowMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;

	HRESULT hr = s_device->CreateTexture2D(&shadowMapDesc, NULL, m_pDepthStencil.reset());
	ASSERT(hr >= 0 && " cannot create depth-stencil uniform buffer");
	if (FAILED(hr))
		return false;
	//Create depth-stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	hr = s_device->CreateDepthStencilState(&dsDesc, m_pDSState.reset());
	if (FAILED(hr))
		return false;
	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	descDSV.Texture2DArray.FirstArraySlice = 0;
	descDSV.Texture2DArray.ArraySize = numCubes * 6;
	descDSV.Texture2DArray.MipSlice = 0;
	descDSV.Flags = NULL;

	hr = s_device->CreateDepthStencilView(m_pDepthStencil, &descDSV, m_pDSV.reset());
	ASSERT(hr >= 0 && " cannot create depth-stencil view");
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	srvDesc.TextureCubeArray.MipLevels = 1;
	srvDesc.TextureCubeArray.MostDetailedMip = 0;
	srvDesc.TextureCubeArray.NumCubes = numCubes;
	srvDesc.TextureCubeArray.First2DArrayFace = 0;

	hr = s_device->CreateShaderResourceView(m_pDepthStencil, &srvDesc, m_pSRV.reset());
	ASSERT(hr >= 0 && " cannot create shader resource view");
	if (FAILED(hr))
		return false;

	return true;
}

bool engine::DepthStencil::initDeferredDepthStencil(uint32_t width, uint32_t height)
{
	//Create depth-stencil buffer using texture resource
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = MULTISAMPLES_COUNT;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = NULL;
	descDepth.MiscFlags = NULL;

	HRESULT hr = s_device->CreateTexture2D(&descDepth, NULL, m_pDepthStencil.reset());
	ASSERT(hr >= 0 && " cannot create depth-stencil uniform buffer");
	if (FAILED(hr))
		return false;
	//Create depth-stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc;

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
	hr = s_device->CreateDepthStencilState(&dsDesc, m_pDSState.reset());
	if (FAILED(hr))
		return false;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.Flags = NULL;
	descDSV.ViewDimension = (MULTISAMPLES_COUNT == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;

	hr = s_device->CreateDepthStencilView(m_pDepthStencil, &descDSV, m_pDSV.reset());
	ASSERT(hr >= 0 && " cannot create depth-stencil view");
	if (FAILED(hr))
		return false;

	return true;
}

bool engine::DepthStencil::initDisabledDepthStencil()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	//Create depth-stencil state

	// Depth test parameters
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.DepthEnable = false;
	HRESULT hr = s_device->CreateDepthStencilState(&dsDesc, m_pDSState.access());
	if (FAILED(hr))
		return false;

	return true;
}

void engine::DepthStencil::bind(uint16_t stencilValue)
{
	s_devcon->OMSetDepthStencilState(m_pDSState, stencilValue);
}

void engine::DepthStencil::bindAsResourcePS(uint16_t inputSlot)
{
	s_devcon->PSSetShaderResources(inputSlot, 1, m_pSRV.access());
}

void engine::DepthStencil::bindAsResourceCS(uint16_t inputSlot)
{
	s_devcon->CSSetShaderResources(inputSlot, 1, m_pSRV.access());
}

void engine::DepthStencil::unbindAsResource(uint16_t inputSlot)
{
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	s_devcon->PSSetShaderResources(inputSlot, 1, &nullSRV[0]);
	s_devcon->CSSetShaderResources(inputSlot, 1, &nullSRV[0]);
}

void engine::DepthStencil::clearView()
{
	s_devcon->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
}

void engine::DepthStencil::clean()
{
	m_pDepthStencil.release();
	m_pDSState.release();
	m_pDSV.release();
	m_pSRV.release();
}
