#pragma once

#include <limits>
#include <cmath>

#include "ray.hpp"
#include "material.hpp"

namespace math
{	
	struct Intersection
	{
		float hitParam;
		XMFLOAT3 point;
		XMFLOAT3 normal;

		Intersection() = default;

		void reset() { hitParam = std::numeric_limits<float>::infinity(); }
		bool exists() { return std::isfinite(hitParam); }

		static Intersection infinite() { Intersection inf; inf.reset(); return inf; }	
	};
}


