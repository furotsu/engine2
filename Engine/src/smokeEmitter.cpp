#include <algorithm>
#include <utility>

#include "smokeEmitter.hpp"
#include "random.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include "utility.hpp"

constexpr float PARTICLE_SPAWN_RATE = 10.0f; //particles per second
constexpr float SMOKE_PARTICLE_MOVE_SPEED = 4.0f; //particle speed per millisecond
constexpr float SMOKE_PARTICLE_GROWTH_SPEED = 1.7f; //particle speed per millisecond
constexpr float SMOKE_PARTICLE_DISPLACEMENT = -1.0f / 300.0f; //particle acceleration for x and z axis TODO

void engine::SmokeEmitter::init(const XMFLOAT3& pos, const XMFLOAT4& tint, float radius, float timePoint)
{
	m_position = pos;
	m_tint = tint;
	m_radius = radius;
	m_timePoint = timePoint;
}

void engine::SmokeEmitter::update(float timePoint)
{
	float deltaTime = timePoint - m_timePoint;
	deleteParticles(timePoint);

	XMFLOAT3 windDir = XMFLOAT3(-0.002f, 0.0f, 0.00015f);
	float speedForTick = SMOKE_PARTICLE_MOVE_SPEED * deltaTime;
	
	updateParticles(deltaTime, speedForTick, windDir);
	
	spawnParticles(timePoint, deltaTime, windDir);

	m_timePoint = timePoint;

}

void engine::SmokeEmitter::deleteParticles(float timePoint)
{
	for (int deletedPos = m_particles.size() - 1; deletedPos >= 0; deletedPos--)
	{
		float deltaParticleTime = timePoint - m_particles[deletedPos].properties.startTimePoint;

		if (deltaParticleTime > 5.0f)
		{
			m_particles.erase(m_particles.begin(), m_particles.begin() + deletedPos + 1);
			break;
		}

		if (deltaParticleTime < 0.5f)
		{
			m_particles[deletedPos].properties.tint.w = 1.0f;
		}
		else if (deltaParticleTime < 4.5f)
		{
			m_particles[deletedPos].properties.tint.w = 1.0f - deltaParticleTime / 4.51f;
		}
	}
}

void engine::SmokeEmitter::updateParticles(float deltaTime, float speedForTick, const XMFLOAT3& windDir)
{
	float growthValue = deltaTime * SMOKE_PARTICLE_GROWTH_SPEED;
	for (auto& particle : m_particles)
	{
		particle.properties.pos = particle.properties.pos + particle.velocity * speedForTick;
		particle.properties.size = particle.properties.size + growthValue;
		particle.velocity = particle.velocity + particle.acceleretion + windDir;
	}
}

void engine::SmokeEmitter::spawnParticles(float timePoint, float deltaTime, const XMFLOAT3& windDir)
{
	m_spawnAmount += deltaTime * PARTICLE_SPAWN_RATE;
	if (!started)
	{
		started = true;
		return;
	}
	for (; m_spawnAmount > 1.0f; m_spawnAmount--)
	{
		float startTimePoint = math::randRangef(m_timePoint, timePoint);

		SmokeParticle particle;
		float posX = math::randRangef(m_position.x - m_radius, m_position.x + m_radius);
		float posZ = math::randRangef(m_position.z - m_radius, m_position.z + m_radius);
		particle.properties.pos = XMFLOAT3(posX, m_position.y, posZ);
		particle.properties.tint = m_tint;
		particle.properties.size = XMFLOAT2(1.5f, 1.5f);
		particle.properties.angle = math::randRangef(0.0f, 180.0f);
		particle.properties.thickness = 0.001f;
		particle.properties.startTimePoint = startTimePoint;

		particle.velocity = XMFLOAT3(0.0f, 1.0f, 0.0f);
		particle.acceleretion = XMFLOAT3( SMOKE_PARTICLE_DISPLACEMENT, 0.0f,  SMOKE_PARTICLE_DISPLACEMENT);

		//should calculate difference in particle position and velocity
		// because it was spawned before current timePoint to fill possible gaps in smoke
		float deltaParticleTime = (timePoint - startTimePoint) * SMOKE_PARTICLE_MOVE_SPEED;
		XMFLOAT3 velocity = particle.velocity + deltaParticleTime * particle.acceleretion;

		particle.properties.pos = particle.properties.pos + deltaParticleTime * velocity ;
		particle.velocity = velocity;
		m_particles.push_back(particle);
	}
}

void engine::SmokeEmitter::clean()
{
	emission = nullptr;
	EMVA = nullptr;
	RLT = nullptr;
	BotBF = nullptr;
}
