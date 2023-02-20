#include "renderTarget.hpp"
#include "constants.hpp"
#include "globals.hpp"

namespace engine
{

	RenderTarget::RenderTarget()
	{
	}

	bool RenderTarget::init(int textureWidth, int textureHeight, DXGI_FORMAT format, uint32_t multisamplesCount)
	{
		if (textureWidth == 0 || textureHeight == 0)
			return false;
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = textureWidth;
		textureDesc.Height = textureHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = multisamplesCount;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		HRESULT result = s_device->CreateTexture2D(&textureDesc, NULL, m_renderTargetTexture.access());
		if (FAILED(result))
			return false;

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = (multisamplesCount > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		result = s_device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, m_renderTargetView.access());
		if (FAILED(result))
			return false;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = (multisamplesCount > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		result = s_device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, m_shaderResourceView.access());
		if (FAILED(result))
			return false;

		return true;
	}

	void RenderTarget::ClearRenderTarget(float r, float g, float b, float a)
	{
		float color[4] = { r, g, b, a };
		
		s_devcon->ClearRenderTargetView(m_renderTargetView, color);
	}


	DxResPtr<ID3D11ShaderResourceView> RenderTarget::GetShaderResourceView()
	{
		return m_shaderResourceView;
	}

	void RenderTarget::SetRenderTarget(DxResPtr<ID3D11DepthStencilView> depthStencilView)
	{
		s_devcon->OMSetRenderTargets(1, m_renderTargetView.access(), depthStencilView);
	}

	void RenderTarget::bindPS(uint32_t slot)
	{
		s_devcon->PSSetShaderResources(slot, 1, m_shaderResourceView.access());
	}

	void RenderTarget::bindCS(uint32_t slot)
	{
		s_devcon->CSSetShaderResources(slot, 1, m_shaderResourceView.access());
	}

	void RenderTarget::unbind(uint32_t slot)
	{
		ID3D11ShaderResourceView* view = nullptr;

		s_devcon->PSSetShaderResources(slot, 1, &view);
		s_devcon->CSSetShaderResources(slot, 1, &view);
	}

	void RenderTarget::clean()
	{
		m_shaderResourceView.release();
		m_renderTargetView.release();
		m_renderTargetTexture.release();
	}


















}