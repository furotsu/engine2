#pragma once

#include <vector>

#include "triangle.hpp"
#include "hitable.hpp"
#include "texture.hpp"
#include "debug.hpp"
#include "material.hpp"
#include "box.hpp"
#include "triangleOctree.hpp"

namespace engine
{
	struct Mesh
	{
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT3 normal;
			XMFLOAT2 texCoords;
			XMFLOAT3 tangent;
			XMFLOAT3 bitangent;
		};

		struct Triangle
		{
			uint32_t indices[3];
		};

		Mesh& operator = (const Mesh&) = delete;

		std::vector<Vertex> vertices;
		std::vector<Triangle> triangles;
		TriangleOctree octree;
		bool indexed;
		Box box;
		XMMATRIX m_transform;
		XMMATRIX m_transformInv;
		std::string name;
		Mesh() = default;

		bool hit(const math::ray& r, math::Intersection& rec, float tMin = RAY_MIN, float tMax = RAY_MAX) const;

		void clean();


	};
}
