#pragma once

#include <memory>
#include <map>
#include <array>

#include "DxRes.hpp"
#include "lightSource.hpp"
#include "modelManager.hpp"
#include "emissive_instances.hpp"
#include "solidVector.hpp"
#include "camera.hpp"

namespace engine
{

	struct LightUniform
	{
		std::array<PointLight::Properties, MAX_POINT_LIGHTS> pointLight;
		std::array<std::array<XMMATRIX, 6>, MAX_POINT_LIGHTS> viewProjMatrices;
	};

	class LightSystem
	{
	protected:
		static LightSystem* s_manager;

		DirectionalLight m_dirLight;
		SolidVector<PointLight> m_pointLights;
		SolidVector<SpotLight> m_spotLights;

		DxResPtr<ID3D11Buffer> m_uniformGlobal;
		LightUniform m_uniform;

	public:
		LightSystem();
		LightSystem(LightSystem& other) = delete;
		void operator=(const LightSystem&) = delete;

		static void init();
		static void deinit();
		static LightSystem* GetInstance();

		void initLightUniforms();

		void bindLights();
		void setLightUniforms(LightUniform& data);
		void updateLightsPositions();
		void updateIrradiance(Camera& camera);

		void addDirectionalLight(DirectionalLight& dirLight);
		void addPointLight(PointLight& pointLight, std::vector<EmissiveInstances::Instance>& instances);
		void addSpotLight(SpotLight& spotLight, std::vector<EmissiveInstances::Instance>& instances);

		float getPointLightsNum();

		XMFLOAT3 irradianceAtDistanceToRadiance(XMFLOAT3 irradiance, float distance, float radius);

		PointLight& getPointLight(uint32_t index);

		static void clean();

		friend class MeshSystem;
		friend class OpaqueInstances;
		friend class DissolutionInstances;
		friend class DisappearanceInstances;
	};
}