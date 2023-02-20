#pragma once

#include "cmath"

#include "ray.hpp"
#include "box.hpp"
#include <DirectXMath.h>
#include "plane.hpp"
using namespace DirectX;

namespace engine
{
	struct MeshIntersection
	{
		math::Plane intersectionPlane;
		XMFLOAT3 offset;
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		float nearP;
		float t;
		uint32_t triangle;
		uint32_t transformID = -1;
		uint16_t instanceID = -1;
		float distToPickedObj;

		constexpr void reset(float nearP, float farP = std::numeric_limits<float>::infinity())
		{
			this->nearP = nearP;
			t = farP;
			distToPickedObj = farP;
		}
		bool valid() const { return std::isfinite(t); }
	};

	struct Ray
	{
		math::ray r;

		bool intersect(float boxT, const Box& box) const;
		bool intersectSphere(MeshIntersection& intersection, const Box& box) const;
		bool intersect(MeshIntersection& intersection, const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3) const;
	};
}