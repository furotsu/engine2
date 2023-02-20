#include "lightSystem.hpp"
#include "modelManager.hpp"
#include "meshSystem.hpp"
#include "transformSystem.hpp"
#include "hlsl.hpp"

namespace engine
{
	LightSystem* LightSystem::s_manager = nullptr;

	LightSystem::LightSystem()
	{
		LightSystem::initLightUniforms();
	}

	void LightSystem::init()
	{
		if (s_manager == nullptr)
		{
			s_manager = new LightSystem();
		}
		else
		{
			ASSERT(false && "Initializing \" LightSystem \" singleton more than once ");
		}
	}

	void LightSystem::deinit()
	{
		if (s_manager == nullptr)
		{
			ASSERT(false && "Trying to delete \" LightSystem \" singleton more than once ");
		}
		else
		{
			clean();
			delete s_manager;
			s_manager = nullptr;
		}
	}

	LightSystem* LightSystem::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ASSERT(false && "Trying to call \" LightSystem \" singleton  instance before initializing it");
		}
#endif
		return s_manager;
	}

	void LightSystem::initLightUniforms()
	{
		//Create global uniform buffer
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

		cbbd.Usage = D3D11_USAGE_DYNAMIC;
		cbbd.ByteWidth = sizeof(LightUniform);
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbbd.MiscFlags = 0;

		HRESULT hr = s_device->CreateBuffer(&cbbd, NULL, m_uniformGlobal.reset());
		ASSERT(hr >= 0 && " cannot create global uniform buffer");
	}

	void LightSystem::bindLights()
	{
		setLightUniforms(m_uniform);
	}

	void LightSystem::setLightUniforms(LightUniform& data)
	{
		D3D11_MAPPED_SUBRESOURCE res;

		s_devcon->Map(m_uniformGlobal, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &res);
		memcpy(res.pData, &data, sizeof(LightUniform));
		s_devcon->Unmap(m_uniformGlobal, NULL);

		s_devcon->VSSetConstantBuffers(PER_FRAME_LIGHT_UNIFORM_SLOT, 1, m_uniformGlobal.access()); 
		s_devcon->PSSetConstantBuffers(PER_FRAME_LIGHT_UNIFORM_SLOT, 1, m_uniformGlobal.access());
	}

	void LightSystem::addDirectionalLight(DirectionalLight& dirLight)
	{
		m_dirLight = dirLight;
	}

	void LightSystem::addPointLight(PointLight& pl, std::vector<EmissiveInstances::Instance>& instances)
	{
		for (uint32_t i = 0; i != instances.size(); ++i)
		{
			pl.positionId = instances[i].transformID;
			XMVECTOR pos = TransformSystem::GetInstance()->getTransform(pl.positionId).r[3];
			pl.viewMatId = TransformSystem::GetInstance()->addPointLightViewMat(pl.positionId);

			pl.properties.position = XMFLOAT3(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos));
			m_uniform.pointLight[i] = pl.properties;

			std::array<XMMATRIX, 6>& a = TransformSystem::GetInstance()->getPointLightViewProjMat(pl.viewMatId);

			m_uniform.viewProjMatrices[i] = TransformSystem::GetInstance()->getPointLightViewProjMat(pl.viewMatId);

			instances[i].type = LightType::POINT;
			instances[i].lightSourceIndex = m_pointLights.insert(pl);
		}

		MeshSystem::GetInstance()->addEmissiveModel(instances);
	}


	void LightSystem::addSpotLight(SpotLight& sl, std::vector<EmissiveInstances::Instance>& instances)
	{
		for (uint32_t i = 0; i != instances.size(); ++i)
		{
			sl.positionId = instances[i].transformID;
			XMVECTOR pos = TransformSystem::GetInstance()->getTransform(sl.positionId).r[3];
			sl.properties.position = XMFLOAT3(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos));

			instances[i].type = LightType::POINT;
			instances[i].lightSourceIndex = m_spotLights.insert(sl);
		}

		MeshSystem::GetInstance()->addEmissiveModel(instances);
	}

	float LightSystem::getPointLightsNum()
	{
		return m_pointLights.size();
	}

	PointLight& LightSystem::getPointLight(uint32_t index)
	{
		return m_pointLights.at(index);
	}

	void LightSystem::updateLightsPositions()
	{
		for (int i = 0; i != m_pointLights.size(); i++)
		{
			XMVECTOR pos = TransformSystem::GetInstance()->getTransform(m_pointLights[i].positionId).r[3];
			m_pointLights[i].properties.position = XMFLOAT3(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos));
			m_uniform.pointLight[i] = m_pointLights[i].properties;

			TransformSystem::GetInstance()->updatePointLightViewProjMat(m_pointLights[i].viewMatId, m_pointLights[i].positionId);

			m_uniform.viewProjMatrices[i] = TransformSystem::GetInstance()->getPointLightViewProjMat(m_pointLights[i].viewMatId);
		}

		for (int i = 0; i != m_spotLights.size(); i++)
		{
			XMVECTOR pos = TransformSystem::GetInstance()->getTransform(m_spotLights[i].positionId).r[3];
			m_spotLights[i].properties.position = XMFLOAT3(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos));
		}

	}

	void LightSystem::updateIrradiance(Camera& camera)
	{
		for (int i = 0; i != m_pointLights.size(); ++i)
		{
			PointLight& pl = m_pointLights.at(i);
			XMFLOAT3 irradiance = pl.properties.color * pl.power;
			float distance2 = math::lengthVec3(pl.properties.position - camera.positionVec3());
			pl.properties.radiance = irradianceAtDistanceToRadiance(irradiance, distance2, pl.properties.radius);
		}

		for (int i = 0; i != m_spotLights.size(); ++i)
		{
			SpotLight& pl = m_spotLights.at(i);
			
			XMFLOAT3 vec = pl.properties.position - camera.positionVec3();
			pl.properties.radiance = irradianceAtDistanceToRadiance(pl.properties.color * pl.power, math::lengthPow2Vec3(vec), pl.properties.radius);
		}
	}

	XMFLOAT3 LightSystem::irradianceAtDistanceToRadiance(XMFLOAT3 irradiance, float distance2, float radius)
	{
		float angleSin = min(1.f, distance2 / (radius * radius));
		float angleCos = sqrtf(1.f - angleSin);
		float occupation = 1.f - angleCos;
		return irradiance / occupation;
	}

	void LightSystem::clean()
	{
	}
}