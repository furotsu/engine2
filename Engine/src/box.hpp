#pragma once

#include <limits>
#include <DirectXMath.h>

#include "utility.hpp"
#include "vec3.hpp"

using namespace DirectX;
namespace engine
{
	struct Box
	{
		XMFLOAT3 min;
		XMFLOAT3 max;

		static constexpr float Inf = std::numeric_limits<float>::infinity();
		static constexpr Box empty() { return  { { Inf, Inf, Inf }, { -Inf, -Inf, -Inf } }; }
		static constexpr Box unit() { return  { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } }; }

		XMFLOAT3 size() const { return max - min; }
		XMFLOAT3 center() const { return (min + max) / 2.f; }
		float radius() const { return math::lengthVec3(size()) / 2.f; }

		void reset()
		{
			constexpr float maxf = std::numeric_limits<float>::infinity();
			min = { maxf , maxf , maxf };
			max = -min;
		}

		void expand(const Box& other)
		{
			min = math::minVec3(min, other.min);
			max = math::maxVec3(max, other.max);
		}

		void expand(const XMFLOAT3& point)
		{
			min = math::minVec3(min, point);
			max = math::maxVec3(max, point);
		}

		bool contains(const XMFLOAT3& P)
		{
			return
				min.x <= P.x && P.x <= max.x &&
				min.y <= P.y && P.y <= max.y &&
				min.z <= P.z && P.z <= max.z;
		}
	};
}