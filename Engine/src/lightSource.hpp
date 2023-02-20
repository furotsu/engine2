#pragma once
#include "constants.hpp"


namespace engine
{
	enum class LightType
	{
		POINT,
		DIRECTIONAL,
		SPOT
	};

	class PointLight
	{
	public:
		struct Properties
		{
			XMFLOAT3 position;
			float radius;
			XMFLOAT3 color;
			float padding1;
			XMFLOAT3 radiance;
			float padding2;
		};
		float power;
		Properties properties;
		uint32_t positionId;
		uint32_t viewMatId;

		PointLight() = default;
		PointLight(uint32_t position, XMFLOAT3& color, XMFLOAT3& radiance, float power = 1.0f,  float radius = LIGHTMODEL_SIZE);
	};

	class DirectionalLight
	{
	public:

		struct Properties
		{
			XMFLOAT3 color;
			float solidAngle;
			XMFLOAT3 direction;
			float power;
		};
		Properties properties;

		DirectionalLight() = default;
		DirectionalLight(XMFLOAT3& direction, XMFLOAT3& color, float solidAngle = 2.0f * G_PI);

	};

	class SpotLight
	{
	public:
		struct Properties
		{
			XMFLOAT3 position;
			float radius;
			XMFLOAT3 direction;
			float innerCutOff;
			XMFLOAT3 radiance;
			float outerCutOff;
			XMFLOAT3 color;
			float padding;
		};
		float power;
		Properties properties;
		uint32_t positionId;
		SpotLight() = default;
		SpotLight(XMFLOAT3& position, XMFLOAT3& color, XMFLOAT3& direction, float innerCutOff, float outerCutOff, float power = 1.0f, float radius = LIGHTMODEL_SIZE);
	};
}