#pragma once

#include <DirectXMath.h>
#include "constants.hpp"
#include "vec3.hpp"
#include "utility.hpp"

using namespace DirectX;

namespace math
{

	struct ray
	{
		XMFLOAT3 origin;
		XMFLOAT3 direction; // always normalized

		ray() = default;
		ray(const XMFLOAT3& origin, const XMFLOAT3& direction)
			: origin(origin), direction(normalizeVec3(direction))
		{
		}

		inline XMFLOAT3 pointAtParameter(float t) const { return origin + t * direction; }

	};

}
