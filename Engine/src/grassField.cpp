#include <algorithm>

#include "poissonGenerator.hpp"
#include "grassField.hpp"

void engine::GrassField::init(float size, const XMFLOAT3& position, std::shared_ptr<Texture> albedo, std::shared_ptr<Texture> normal, std::shared_ptr<Texture> opacity, std::shared_ptr<Texture> occlusion, std::shared_ptr<ShaderProgram> shader, float timePoint)
{
	m_size = size;
	m_position = position;
	m_grassAlbedoTexture = albedo;
	m_grassNormalTexture = normal;
	m_grassOpacityTexture = opacity;
	m_grassOcclusionTexture = occlusion;
	m_startTimePoint = timePoint;
	m_shader = shader;

	PoissonGenerator::DefaultPRNG PRNG;
	std::vector<PoissonGenerator::Point> poissonPoints = PoissonGenerator::generatePoissonPoints(GRASS_BUSHES_AMOUNT, PRNG);

	for (auto& point : poissonPoints)
	{
		GrassBush bush;
		bush.position.x = point.x * size;
		bush.position.y = m_position.y + 10.0f;
		bush.position.z = point.y * size;
		bush.size = XMFLOAT2(10.0f, 10.0f); 
		bushesInstances.push_back(bush);
		bushesInstances.push_back(bush);
		bushesInstances.push_back(bush);
		bushesInstances.push_back(bush);
	}

	bushesInstanceBuffer.init(bushesInstances.size() * sizeof(GrassBush), D3D11_USAGE_DYNAMIC);
}

void engine::GrassField::render(float timePoint, const XMFLOAT3& cameraPos)
{
	m_shader->bind();

	std::sort(bushesInstances.begin(), bushesInstances.end(),
		[&cameraPos](const GrassBush& a, const GrassBush& b) -> bool
		{
			return math::lengthVec3(a.position - cameraPos) > math::lengthVec3(b.position - cameraPos);
		});

	auto mapping = bushesInstanceBuffer.map();
	
	memcpy(mapping.pData, bushesInstances.data(), bushesInstances.size() * sizeof(GrassBush));
	
	bushesInstanceBuffer.unmap();

	m_grassAlbedoTexture->bind(0);
	m_grassNormalTexture->bind(1);
	m_grassOpacityTexture->bind(2);

	bushesInstanceBuffer.bind(0, sizeof(GrassBush));
	s_devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	s_devcon->DrawInstanced(24, bushesInstances.size(), 0, 0);

}

void engine::GrassField::clean()
{
	m_shader = nullptr;
	bushesInstanceBuffer.clean();
	bushesInstances.clear();
}
