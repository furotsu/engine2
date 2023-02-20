#include "lightSource.hpp"

engine::PointLight::PointLight(uint32_t position, XMFLOAT3& color, XMFLOAT3& radiance, float power, float radius)
{
	positionId = position;
	properties.color = color;
	this->power = power;
	properties.radius = radius;
	properties.radiance = radiance;
	
}

engine::DirectionalLight::DirectionalLight(XMFLOAT3& direction, XMFLOAT3& color, float solidAngle)
{
	properties.direction = direction;
	properties.color = color;
	properties.solidAngle = solidAngle;
}

engine::SpotLight::SpotLight(XMFLOAT3& position, XMFLOAT3& color, XMFLOAT3& direction, float innerCutOff, float outerCutOff, float power, float radius)
{
	positionId = positionId;
	properties.position = position;
	properties.color = color;
	properties.direction = direction;
	properties.innerCutOff = innerCutOff;
	properties.outerCutOff = outerCutOff;
	power = power;
	properties.radius = radius;
}
