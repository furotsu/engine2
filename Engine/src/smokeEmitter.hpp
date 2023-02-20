#pragma once

#include <vector>

#include "particle.hpp"
#include "textureManager.hpp"

namespace engine
{
	class SmokeEmitter
	{
		std::vector<SmokeParticle> m_particles;
		XMFLOAT4 m_tint;
		XMFLOAT3 m_position;
		float m_radius;
		float m_timePoint;
		float m_particleLifetime;
		float m_spawnAmount = 0;
		bool started = false;

	public:
		std::shared_ptr<Texture> emission;
		std::shared_ptr<Texture> EMVA;
		std::shared_ptr<Texture> RLT;
		std::shared_ptr<Texture> BotBF;

		SmokeEmitter() = default;

		void init(const XMFLOAT3& pos, const XMFLOAT4& tint, float radius, float timePoint);

		void update(float timePoint);
		void deleteParticles(float timePoint);
		void updateParticles(float deltaTime, float speedForTick, const XMFLOAT3& windDir);
		void spawnParticles(float timePoint, float deltaTime, const XMFLOAT3& windDir);
		void clean();
		const inline std::vector<SmokeParticle>& getParticles() { return m_particles; }
	};
}