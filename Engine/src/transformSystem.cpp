#include "modelManager.hpp"
#include "shaderManager.hpp"
#include "transformSystem.hpp"


namespace engine
{
	TransformSystem* TransformSystem::s_manager = nullptr;

	TransformSystem::TransformSystem()
	{
		m_lightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), 1.0f, CAMERA_FAR_PLANE, CAMERA_NEAR_PLANE);
	}

	void TransformSystem::init()
	{

		if (s_manager == nullptr)
		{
			s_manager = new TransformSystem();
		}
		else
		{
			ASSERT(false && "Initializing \" TransformSystem \" singleton more than once ");
		}
	}

	void TransformSystem::deinit()
	{
		if (s_manager == nullptr)
		{
			ASSERT(false && "Trying to delete \" TransformSystem \" singleton more than once ");
		}
		else
		{
			clean();
			delete s_manager;
			s_manager = nullptr;
		}
	}

	TransformSystem* TransformSystem::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ERROR("Trying to call \" TransformSystem \" singleton  instance before initializing it");
		}
#endif
		return s_manager;
	}

	uint32_t TransformSystem::addTransform(const XMMATRIX& transform, uint32_t parentIndex)
	{
		m_inverseTransforms.insert(XMMatrixInverse(nullptr, transform));
		m_chainedTransforms.insert({});
		uint32_t index = m_transforms.insert(transform);
		if (parentIndex != INVALID_PARENT_INDEX)
		{
			m_chainedTransforms.at(parentIndex).push_back(index);
		}

		return index;
	}

	uint32_t TransformSystem::addPointLightViewMat(uint32_t index)
	{
		XMVECTOR& position = m_transforms.at(index).r[3];
		std::array<XMMATRIX, 6> mats;

		XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR left = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR top = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR bottom = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		XMVECTOR forward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		XMVECTOR backward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		mats[0] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + right, top), m_lightProj);
		mats[1] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + left, top), m_lightProj);
		mats[2] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + top, forward), m_lightProj);
		mats[3] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + bottom, backward), m_lightProj);
		mats[4] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + backward, top), m_lightProj);
		mats[5] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + forward, top), m_lightProj);

		return m_lightViewMat.insert(mats);
	}

	void TransformSystem::updatePointLightViewProjMat(uint32_t viewMatIndex, uint32_t lightTransformIndex)
	{
		XMVECTOR& position = m_transforms.at(lightTransformIndex).r[3];
		std::array<XMMATRIX, 6> mats;

		XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR left = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR top = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR bottom = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		XMVECTOR forward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		XMVECTOR backward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		mats[0] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + right, top), m_lightProj);
		mats[1] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + left, top), m_lightProj);
		mats[2] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + top, forward), m_lightProj);
		mats[3] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + bottom, backward), m_lightProj);
		mats[4] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + backward, top), m_lightProj);
		mats[5] = XMMatrixMultiply(XMMatrixLookAtLH(position, position + forward, top), m_lightProj);

		m_lightViewMat.at(viewMatIndex) = mats;
	}

	XMMATRIX TransformSystem::getTransform(uint32_t index)
	{
		return m_transforms.at(index);
	}

	XMMATRIX TransformSystem::getTransformInverse(uint32_t index)
	{
		return m_inverseTransforms.at(index);
	}

	std::array<XMMATRIX, 6>& TransformSystem::getPointLightViewProjMat(uint32_t index)
	{
		return m_lightViewMat.at(index);
	}

	XMVECTOR TransformSystem::getPosition(uint32_t index)
	{
		return m_transforms.at(index).r[3];
	}

	XMFLOAT3 TransformSystem::getPositionVec3(uint32_t index)
	{
		XMMATRIX& m = m_transforms.at(index);;
		XMFLOAT3 pos = XMFLOAT3(XMVectorGetX(m.r[3]), XMVectorGetY(m.r[3]), XMVectorGetZ(m.r[3]));
		return pos;
	}

	void TransformSystem::moveTo(uint32_t position, const XMFLOAT3& point)
	{
		XMVECTOR transformPosition = m_transforms.at(position).r[3];
		XMVECTOR offset = -transformPosition + XMLoadFloat3(&point);
		m_transforms.at(position).r[3] += XMVectorSet(XMVectorGetX(offset), XMVectorGetY(offset), XMVectorGetZ(offset), 0.0f);
		m_inverseTransforms.at(position) = XMMatrixInverse(nullptr, m_transforms.at(position));

		for (uint32_t pos : m_chainedTransforms.at(position))
		{
			XMVECTOR v1 = m_transforms.at(pos).r[3];
			XMVECTOR v2 = m_transforms.at(position).r[3];
			XMFLOAT3 offs = XMFLOAT3(XMVectorGetX(v1) - XMVectorGetX(v2), XMVectorGetY(v1) - XMVectorGetY(v2), XMVectorGetZ(v1) - XMVectorGetZ(v2));
			m_transforms.at(pos).r[3] += XMVectorSet(XMVectorGetX(offset), XMVectorGetY(offset), XMVectorGetZ(offset), 0.0f);
			m_inverseTransforms.at(pos) = XMMatrixInverse(nullptr, m_transforms.at(pos));

		}
	}

	void TransformSystem::moveBy(uint32_t index, const XMFLOAT3& offset)
	{
		m_transforms.at(index).r[3] = XMVectorAdd(m_transforms.at(index).r[3], XMVectorSet(offset.x, offset.y, offset.z, 0.0f));
		m_inverseTransforms.at(index) = XMMatrixInverse(nullptr, m_transforms.at(index));

		for (uint32_t pos : m_chainedTransforms.at(index))
		{
			moveBy(pos, offset);
		}
	}

	void TransformSystem::scaleBy(uint32_t index, const XMFLOAT3& scale)
	{
		m_transforms.at(index) = XMMatrixMultiply(m_transforms.at(index), XMMatrixScaling(scale.x, scale.y, scale.z));
		m_inverseTransforms.at(index) = XMMatrixInverse(nullptr, m_transforms.at(index));
	}

	void TransformSystem::changeTransform(uint32_t index, const XMMATRIX& transform)
	{
		m_transforms.at(index) =  transform;
		m_inverseTransforms.at(index) = XMMatrixInverse(nullptr, m_transforms.at(index));
	}

	void TransformSystem::clean()
	{
		GetInstance()->m_transforms.clear();
	}
}