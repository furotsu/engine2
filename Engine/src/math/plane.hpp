#pragma once

#include "hitable.hpp"
#include "ray.hpp"

namespace math
{
	class Plane
	{
		XMVECTOR m_normal;
		XMVECTOR m_center;

	public:
		Plane() = default;

		Plane(XMVECTOR normal, XMVECTOR centerPos);

		Plane(const Plane&) = default;

		Plane& operator=(const Plane&);

		bool hit(const ray& r, Intersection& hr);
	};
}