#pragma once

#include <vector>
#include <memory>
#include "cmath"

#include "box.hpp"
#include "worldRay.hpp"

namespace engine
{
	struct Mesh;

	class TriangleOctree
	{
	public:
		const static int PREFFERED_TRIANGLE_COUNT;
		const static float MAX_STRETCHING_RATIO;

		TriangleOctree() = default;
		TriangleOctree& operator = (const TriangleOctree&) = delete;

		void clear() { m_mesh = nullptr; }
		bool inited() const { return m_mesh != nullptr; }

		void initialize(const Mesh& mesh);

		bool intersect(const Ray& ray, MeshIntersection& nearest) const;
		bool intersectSphere(const Ray& ray, MeshIntersection& nearest) const;

	protected:
		const Mesh* m_mesh = nullptr;
		std::vector<uint32_t> m_triangles;

		Box m_box;
		Box m_initialBox;

		std::shared_ptr<std::array<TriangleOctree, 8>> m_children; 

		void initialize(const Mesh& mesh, const Box& parentBox, const XMFLOAT3& parentCenter, int octetIndex);

		bool addTriangle(uint32_t triangleIndex, const XMFLOAT3& V1, const XMFLOAT3& V2, const XMFLOAT3& V3, const XMFLOAT3& center);

		bool intersectInternal(const Ray& ray, MeshIntersection& nearest) const;
	};
}