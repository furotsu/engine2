#pragma once

#include <memory>
#include <vector>

#include "DxRes.hpp"
#include "modelManager.hpp"
#include "renderTarget.hpp"
#include "window.hpp"

#include <DirectXMath.h>
using namespace DirectX;

namespace engine
{
	struct Decal
	{
		struct Properties
		{
			XMMATRIX transform;
			XMMATRIX inverseTransform;
			XMFLOAT3 albedo;
			float roughness;
			float metalness;
			uint16_t objID;
		};
		Properties properties;
		uint32_t transformID;
	};

	class DecalSystem
	{
	protected:
		static DecalSystem* s_manager;

		std::shared_ptr<ShaderProgram> m_decalShader;
		uint32_t pixelBufferID;

		std::vector<Decal> m_decals;
		std::shared_ptr<Model> m_cubeModel;
		VertexBuffer<Decal::Properties> instanceBuffer;
		std::shared_ptr<Texture> m_defaultNormalTexture;
		RenderTarget m_objTextureCopy;

	public:
		DecalSystem();
		DecalSystem(DecalSystem& other) = delete;
		void operator=(const DecalSystem&) = delete;

		static void init();
		static void deinit();
		static DecalSystem* GetInstance();
		void initObjTextureCopy(const Window& window);
		void addDecal(const XMMATRIX& modelTransform, uint32_t parentTransformId, uint16_t instanceID);
		void setDefaultNormalTexture(std::shared_ptr<Texture> NormalTexture);

		void updateInstanceBuffers();

		void render(RenderTarget& objIDTarget, RenderTarget& normalTextureTarget);
		static void clean();
	};
}