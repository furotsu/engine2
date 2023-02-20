#include "shadowSystem.hpp"
#include "debug.hpp"
#include "meshSystem.hpp"

namespace engine
{
	ShadowSystem* ShadowSystem::s_manager = nullptr;

	ShadowSystem::ShadowSystem()
	{
	}

	void ShadowSystem::init()
	{
		if (s_manager == nullptr)
		{
			s_manager = new ShadowSystem();
		}
		else
		{
			ASSERT(false && "Initializing \" ShadowSystem \" singleton more than once ");
		}
	}

	void ShadowSystem::deinit()
	{
		if (s_manager == nullptr)
		{
			ASSERT(false && "Trying to delete \" ShadowSystem \" singleton more than once ");
		}
		else
		{
			GetInstance()->clean();
			delete s_manager;
			s_manager = nullptr;
		}
	}

	ShadowSystem* ShadowSystem::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ERROR("Trying to call \" ShadowSystem \" singleton  instance before initializing it");
		}
#endif
		return s_manager;
	}

	uint32_t ShadowSystem::createShadowMap(uint32_t pointLightNum)
	{
		DepthStencil depthStencil;

		depthStencil.init(SHADOWMAP_SIDE_SIZE, SHADOWMAP_SIDE_SIZE, DepthStencil::Type::Cubemap, pointLightNum);

		return m_depthStencils.insert(depthStencil);
	}

	void ShadowSystem::renderToShadowMap(uint32_t shadowMapIndex, const Camera& camera)
	{
		ID3D11ShaderResourceView* view = nullptr;

		//Unbind shadowMap depthStencil from shaderResourceView to write new values to it
		s_devcon->PSSetShaderResources(SHADOWMAP_RESOURCE_SLOT, 1, &view);

		s_devcon->OMSetRenderTargets(0, 0, m_depthStencils.at(shadowMapIndex).getDepthStencilView());
		s_devcon->ClearDepthStencilView(m_depthStencils.at(shadowMapIndex).getDepthStencilView(), D3D11_CLEAR_DEPTH, 0.0f, 0);
		m_depthStencils.at(shadowMapIndex).bind();

		MeshSystem::GetInstance()->renderDepthOnly(camera);
	}

	void ShadowSystem::bindMapAsResource(uint32_t shadowMapIndex, uint16_t resourseSlot)
	{
		s_devcon->PSSetShaderResources(resourseSlot, 1, m_depthStencils.at(shadowMapIndex).getShaderResourseView().access());
	}

	void ShadowSystem::unbindMapAsResource(uint16_t resourseSlot)
	{
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		s_devcon->PSSetShaderResources(resourseSlot, 1, &nullSRV[0]);
	}
	void ShadowSystem::clean()
	{
		for (int i = 0; i != m_depthStencils.size(); i++)
			m_depthStencils.at(i).clean();
	}
}