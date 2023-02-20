#pragma once

#include <array>

#include "renderTarget.hpp"
#include "window.hpp"

namespace engine
{

	struct GBuffer
	{
		std::array<RenderTarget, 5> gBufferTargets;
		std::vector<ID3D11RenderTargetView*> gBufferRTV;

		GBuffer() = default;
		void init(const Window& window);

		void bind(DxResPtr<ID3D11DepthStencilView> depthStencil);
		void bindAsTextures(uint32_t startSlot);
		void unbindAsTextures(uint32_t startSlot);
		void clearRenderTargets();

		RenderTarget& getNormalTarget();
		RenderTarget& getAlbedoTarget();
		RenderTarget& getObjIDTarget();

		void clean();
	};
}