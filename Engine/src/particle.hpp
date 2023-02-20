#pragma once

#include "vec3.hpp"

namespace engine
{
	struct SmokeParticle
	{
		struct Properties
		{
			XMFLOAT3 pos;
			XMFLOAT4 tint;
			XMFLOAT2 size;
			float thickness;
			float angle;
			float startTimePoint;
		};
		Properties properties;
		XMFLOAT3 velocity;
		XMFLOAT3 acceleretion;
	};
}