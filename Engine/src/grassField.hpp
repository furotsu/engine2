#pragma once

#include <memory>
#include <vector>

#include "vec3.hpp"
#include "shaderManager.hpp"
#include "textureManager.hpp"
#include "model.hpp"

constexpr uint32_t GRASS_BUSHES_AMOUNT = 1000;

namespace engine
{
	struct GrassBush
	{
		XMFLOAT3 position;
		XMFLOAT2 size;
	};

	class GrassField
	{

		std::shared_ptr<ShaderProgram> m_shader;

		VertexBuffer<GrassBush> bushesInstanceBuffer;
		std::vector<GrassBush> bushesInstances;

		float m_size;
		XMFLOAT3 m_position;
		std::shared_ptr<Texture> m_grassAlbedoTexture;
		std::shared_ptr<Texture> m_grassNormalTexture;
		std::shared_ptr<Texture> m_grassOpacityTexture;
		std::shared_ptr<Texture> m_grassOcclusionTexture;
		float m_startTimePoint;

	public:
		GrassField() = default;

		void init(float size, const XMFLOAT3& position, std::shared_ptr<Texture> albedo, std::shared_ptr<Texture> normal, std::shared_ptr<Texture> opacity, std::shared_ptr<Texture> occlusion, std::shared_ptr<ShaderProgram> shader, float timePoint);

		void render(float timePoint, const XMFLOAT3& cameraPos);
		void clean();
	};

}
