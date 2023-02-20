#pragma once

#include "hitable.hpp"

using namespace DirectX;

namespace math
{
	class Triangle
	{
	public:

		XMVECTOR vertices[3];
		XMVECTOR normal;

		Triangle() = default;
		Triangle(XMVECTOR x, XMVECTOR y, XMVECTOR z, XMVECTOR normal)
		{
			vertices[0] = x;
			vertices[1] = y;
			vertices[2] = z;

			this->normal = XMVector3Normalize(normal);
		}

		bool hit(const ray& r, Intersection& rec, float tMin = RAY_MIN, float tMax = RAY_MAX) const;
	};
}