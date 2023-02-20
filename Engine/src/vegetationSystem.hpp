#pragma once

#include <memory>
#include <vector>

#include "DxRes.hpp"
#include "grassField.hpp"

#include <DirectXMath.h>
using namespace DirectX;

namespace engine
{
	class VegetationSystem
	{
	protected:
		static VegetationSystem* s_manager;

		std::shared_ptr<ShaderProgram> m_grassShader;
		std::vector<GrassField> m_grassFields;

	public:
		VegetationSystem();
		VegetationSystem(VegetationSystem& other) = delete;
		void operator=(const VegetationSystem&) = delete;

		static void init();
		static void deinit();
		static VegetationSystem* GetInstance();

		void addGrassField(float size, const XMFLOAT3& position, std::shared_ptr<Texture> albedo, std::shared_ptr<Texture> normal, std::shared_ptr<Texture> opacity, std::shared_ptr<Texture> occlusion, float timePoint);

		void render(float timePoint, const XMFLOAT3& cameraPos);

		static void clean();
	};
}