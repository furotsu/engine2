#pragma once

#include <memory>
#include <array>

#include "DxRes.hpp"
#include "solidVector.hpp"

#include <DirectXMath.h>
using namespace DirectX;

constexpr uint32_t INVALID_PARENT_INDEX = -1;

namespace engine
{
	class TransformSystem
	{
	private:
		SolidVector<std::vector<uint32_t>> m_chainedTransforms;

	protected:
		static TransformSystem* s_manager;

		SolidVector<XMMATRIX> m_transforms;
		SolidVector<XMMATRIX> m_inverseTransforms;
		SolidVector<std::array<XMMATRIX, 6>> m_lightViewMat;
		XMMATRIX m_lightProj;

	public:
		TransformSystem();
		TransformSystem(TransformSystem& other) = delete;
		void operator=(const TransformSystem&) = delete;

		static void init();
		static void deinit();
		static TransformSystem* GetInstance();

		uint32_t addTransform(const XMMATRIX& transform, uint32_t parentIndex = INVALID_PARENT_INDEX);
		uint32_t addPointLightViewMat(uint32_t index);
		void updatePointLightViewProjMat(uint32_t viewMatIndex, uint32_t lightTransformIndex);

		XMMATRIX getTransform(uint32_t index);
		XMMATRIX getTransformInverse(uint32_t index);
		std::array<XMMATRIX, 6>& getPointLightViewProjMat(uint32_t index);

		XMVECTOR getPosition(uint32_t index);
		XMFLOAT3 getPositionVec3(uint32_t index);

		void moveTo(uint32_t index, const XMFLOAT3& point);
		void moveBy(uint32_t index, const XMFLOAT3& offset);
		void scaleBy(uint32_t index, const XMFLOAT3& scale);
		void changeTransform(uint32_t index, const XMMATRIX& transform);

		static void clean();
	};
}