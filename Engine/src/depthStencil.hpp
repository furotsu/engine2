#pragma once

#include "DxRes.hpp"

namespace engine
{
	class DepthStencil
	{
	public:
		enum class Type { DepthStencil, ShadowDepth, Cubemap, Deferred, Disabled};
	
	private:
		Type m_type;
		uint32_t width;
		uint32_t height;

		DxResPtr<ID3D11Texture2D> m_pDepthStencil;
		DxResPtr<ID3D11DepthStencilState> m_pDSState;
		DxResPtr<ID3D11DepthStencilView> m_pDSV;
		DxResPtr<ID3D11ShaderResourceView> m_pSRV;

	public:
		DepthStencil() = default;

		bool init(uint32_t width, uint32_t height, DepthStencil::Type type, uint32_t numCubes = 1);

		bool initDefaultDepthStencil(uint32_t width, uint32_t height);
		bool initShadowDepthStencil(uint32_t width, uint32_t height);
		bool initCubemapDepthStencil(uint32_t width, uint32_t height, uint32_t numCubes);
		bool initDeferredDepthStencil(uint32_t width, uint32_t height);
		bool initDisabledDepthStencil();
		void bind(uint16_t stencilValue = 1u);
		void bindAsResourcePS(uint16_t inputSlot);
		void bindAsResourceCS(uint16_t inputSlot);
		void unbindAsResource(uint16_t inputSlot);
		void clearView();

		DxResPtr<ID3D11DepthStencilView> getDepthStencilView() { return m_pDSV; }
		DxResPtr<ID3D11ShaderResourceView> getShaderResourseView() { return m_pSRV; }
		DxResPtr<ID3D11Texture2D> getRenderTargetRes() { return m_pDepthStencil; } 


		void clean();
	};
}