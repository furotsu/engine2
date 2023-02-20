#pragma once

#include <memory>
#include <vector>

#include "DxRes.hpp"
#include "smokeEmitter.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "sky.hpp"
#include "depthStencil.hpp"
#include "gbuffer.hpp"

#include <DirectXMath.h>
using namespace DirectX;

namespace engine
{
	class ParticleSystem
	{
	protected:
		static ParticleSystem* s_manager;

		std::vector<SmokeEmitter> m_smokeEmitters;
		std::shared_ptr<ShaderProgram> m_smokeShader;
		
		VertexBuffer<SmokeParticle> particleInstanceBuffer;

		DxResPtr<ID3D11Buffer> particlesData;
		DxResPtr<ID3D11Buffer> particlesRange;
		DxResPtr<ID3D11Buffer> indirectArgsBuffer;
		DxResPtr<ID3D11UnorderedAccessView> particlesDataUAV;
		DxResPtr<ID3D11UnorderedAccessView> particlesRangeUAV;
		DxResPtr<ID3D11UnorderedAccessView> indirectArgsBufferUAV;
		DxResPtr<ID3D11ShaderResourceView> particlesDataSRV;
		DxResPtr<ID3D11ShaderResourceView> particlesRangeSRV;

		std::shared_ptr<ShaderProgram> particleSpawner;
		std::shared_ptr<ShaderProgram> particleUpdater;
		std::shared_ptr<ShaderProgram> particleRangeUpdater;
		std::shared_ptr<ShaderProgram> particlesDrawer;
		std::shared_ptr<Texture> m_sparkTexture;

	public:
		ParticleSystem();
		ParticleSystem(ParticleSystem& other) = delete;
		void operator=(const ParticleSystem&) = delete;

		static void init();
		static void deinit();
		static ParticleSystem* GetInstance();

		void initDisappearanceParticlesData();
		void addSmokeEmitter(const XMFLOAT3& pos, const XMFLOAT4& tint, float radius, float timePoint, std::shared_ptr<Texture> emission, std::shared_ptr<Texture> EMVA, std::shared_ptr<Texture> RLT, std::shared_ptr<Texture> BotBF);
		void setParticleTexture(std::shared_ptr<Texture> sparkTexture);
		void update(float timePoint);

		void render(const XMFLOAT3& cameraPos);

		void renderDisappearanceParticles(Sky& sky, DepthStencil& depthStencil, DepthStencil& disabledDepthStencil, GBuffer& gbuffer, RenderTarget& renderTarget);

		static void clean();
	};
}