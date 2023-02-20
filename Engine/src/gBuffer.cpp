#include "gBuffer.hpp"
#include "debug.hpp"
#include "constants.hpp"
#include "globals.hpp"

void engine::GBuffer::init(const Window& window)
{
	RenderTarget normalTarget;
	RenderTarget albedoTarget;
	RenderTarget roughMetallTarget;
	RenderTarget emissionTarget;
	RenderTarget objectIdTarget;

	ALWAYS_ASSERT(normalTarget.init(window.m_width, window.m_height, DXGI_FORMAT_R16G16_SNORM,                MULTISAMPLES_COUNT));
	ALWAYS_ASSERT(albedoTarget.init(window.m_width, window.m_height,       DXGI_FORMAT_R8G8B8A8_UNORM,        MULTISAMPLES_COUNT));
	ALWAYS_ASSERT(roughMetallTarget.init(window.m_width, window.m_height,  DXGI_FORMAT_R8G8_UNORM,            MULTISAMPLES_COUNT));
	ALWAYS_ASSERT(emissionTarget.init(window.m_width, window.m_height,     DXGI_FORMAT_R16G16B16A16_FLOAT,    MULTISAMPLES_COUNT));
	ALWAYS_ASSERT(objectIdTarget.init(window.m_width, window.m_height,     DXGI_FORMAT_R16_UINT,              MULTISAMPLES_COUNT));

	gBufferTargets[0]  = normalTarget;
	gBufferTargets[1]  = albedoTarget;
	gBufferTargets[2]  = roughMetallTarget;
	gBufferTargets[3]  = emissionTarget;
	gBufferTargets[4]  = objectIdTarget;
	gBufferRTV.clear();
	gBufferRTV.insert(gBufferRTV.end(), {normalTarget.getRenderTargetView(), albedoTarget.getRenderTargetView(), roughMetallTarget.getRenderTargetView(), emissionTarget.getRenderTargetView(), objectIdTarget.getRenderTargetView() });
}

void engine::GBuffer::bind(DxResPtr<ID3D11DepthStencilView> depthStencil)
{

	s_devcon->OMSetRenderTargets(5, &gBufferRTV[0], depthStencil);
}

void engine::GBuffer::bindAsTextures(uint32_t startSlot)
{
	for (int i = 0; i != gBufferTargets.size(); i++)
	{
		gBufferTargets[i].bindPS(startSlot + i);
	}
}

void engine::GBuffer::unbindAsTextures(uint32_t startSlot)
{
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	for (int i = 0; i != gBufferTargets.size(); i++)
	{
		s_devcon->PSSetShaderResources(startSlot + i, 1, &nullSRV[0]);
	}
}

void engine::GBuffer::clearRenderTargets()
{
	for (auto& target : gBufferTargets)
		target.ClearRenderTarget();
}

engine::RenderTarget& engine::GBuffer::getNormalTarget()
{
	return gBufferTargets[0];
}

engine::RenderTarget& engine::GBuffer::getAlbedoTarget()
{
	return gBufferTargets[1];
}

engine::RenderTarget& engine::GBuffer::getObjIDTarget()
{
	return gBufferTargets[4];
}

void engine::GBuffer::clean()
{
	for (auto& renderTarget : gBufferTargets)
		renderTarget.clean();
}
