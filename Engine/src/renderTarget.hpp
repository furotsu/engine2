#pragma once

#include "DxRes.hpp"

namespace engine
{
	class RenderTarget
	{
	private:
		DxResPtr<ID3D11Texture2D> m_renderTargetTexture;
		DxResPtr<ID3D11RenderTargetView> m_renderTargetView;
		DxResPtr<ID3D11ShaderResourceView> m_shaderResourceView;

	public:
		RenderTarget();

		bool init(int width, int height, DXGI_FORMAT format, uint32_t multisamplesCount);

		void SetRenderTarget(DxResPtr<ID3D11DepthStencilView>);
		void ClearRenderTarget(float r = 0.0f, float g = 0.2f, float b = 0.4f, float a = 1.0f);
		DxResPtr<ID3D11RenderTargetView> getRenderTargetView() { return m_renderTargetView; }
		DxResPtr<ID3D11ShaderResourceView> GetShaderResourceView();
		DxResPtr<ID3D11Texture2D> getRenderTargetRes() { return m_renderTargetTexture; }
		void bindPS(uint32_t slot = 0u);
		void bindCS(uint32_t slot = 0u);
		void unbind(uint32_t slot = 0u);

		void clean();
	};
}