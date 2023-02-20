#pragma once

#include "camera.hpp"
#include "window.hpp"
#include "depthStencil.hpp"
#include "solidVector.hpp"	
#include "constants.hpp"

namespace engine
{
	class ShadowSystem
	{
	protected:
		static ShadowSystem* s_manager;

		SolidVector<DepthStencil> m_depthStencils;

	public:
		ShadowSystem();
		ShadowSystem(ShadowSystem& other) = delete;
		void operator=(const ShadowSystem&) = delete;

		static void init();
		static void deinit();
		static ShadowSystem* GetInstance();

		uint32_t createShadowMap(uint32_t pointLightNum);
		void renderToShadowMap(uint32_t shadowMapIndex, const Camera& camera);
		void bindMapAsResource(uint32_t shadowMapIndex, uint16_t resourseSlot = SHADOWMAP_RESOURCE_SLOT);
		void unbindMapAsResource(uint16_t resourseSlot);
		
		void clean();
	};
}