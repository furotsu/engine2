#pragma once

#include "ray.hpp"
#include "vec3.hpp"
#include "hitable.hpp"

namespace math
{
	class Plane
	{
		XMFLOAT3 m_normal;
		XMFLOAT3 m_center;

	public:
		Plane() = default;

		Plane(XMFLOAT3 normal, XMFLOAT3 centerPos);

		Plane(const Plane&) = default;

		Plane& operator=(const Plane&);

		void setNormal(const XMFLOAT3& normal);
		void setNormal(const XMVECTOR& normal);
		void moveCenter(const XMVECTOR& offset);
		void setCenter(const XMFLOAT3& position);
		bool hit(const ray& r, Intersection& hr);
		inline XMFLOAT3 getCenter() { return m_center; }
	};
}