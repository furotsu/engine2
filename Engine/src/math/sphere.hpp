#pragma once

#include "hitable.hpp"

namespace math
{
	class Sphere
	{
	public:
		XMVECTOR center;
		float radius;

		Sphere() = default;
		Sphere(XMVECTOR center, float radius)
			: center(center), radius(radius)
		{
		}

		bool hit(const ray& r, Intersection& rec, float tMin = RAY_MIN, float tMax = RAY_MAX) const;

		inline XMVECTOR getCenter() const { return center; }
		inline float getRadius() const { return radius; }

	};

}

