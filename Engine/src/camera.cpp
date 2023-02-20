#include "camera.hpp"

namespace engine
{

	void Camera::setPerspective(float fov, float width, float height, float nearP, float farP) // sets m_proj and m_projInv
	{
		m_proj = XMMatrixPerspectiveFovLH(fov, width / height, farP, nearP);

		m_projInv = XMMatrixInverse(nullptr, m_proj);

		m_updatedMatrices = false;
		updateMatrices();
	}

	void Camera::setWorldOffset(const XMVECTOR& offset)
	{
		m_updatedMatrices = false;

		// overwrites
		m_viewInv.r[3] = offset;

		transformation.position = offset;
	}

	void Camera::addWorldOffset(const XMVECTOR& offset)
	{
		m_updatedMatrices = false;

		m_viewInv.r[3] += offset;

		transformation.position += offset;
	}

	void Camera::addRelativeOffset(const XMVECTOR& offset)
	{
		updateBasis(); // requires m_rotation to be decoded into 3x3 matrix stored in m_viewInv
		m_updatedMatrices = false;

		transformation.position += offset;

		m_viewInv.r[3] += XMVectorGetX(offset) * right() + XMVectorGetY(offset) * top() + XMVectorGetZ(offset) * forward();

	}

	void Camera::setWorldAngles(const Angles& angles)
	{
		m_updatedBasis = false;
		m_updatedMatrices = false;

		transformation.rotation = math::Quaternion({ 1.0f, 0.0f, 0.0f, 0.0f }, angles.pitch);
		transformation.rotation *= math::Quaternion({ 0.0f, 1.0f, 0.0f, 0.0f }, angles.yaw);

		if (m_rollEnabled)
		{
			transformation.rotation *= math::Quaternion({ 0.0f, 0.0f, 1.0f, 0.0f }, angles.roll);
		}

		m_angles = angles;

		math::normalizeQuat(transformation.rotation);

		// and keep the quaternion of length 1.0, so it represents a valid 3D rotation.
		// Also we can do it only from time to time, like every 100th call of rotating functions.
	}

	void Camera::addWorldAngles(const Angles& angles)
	{
		m_updatedBasis = false;
		m_updatedMatrices = false;

		transformation.rotation *= math::Quaternion({ 1.0f, 0.0f, 0.0f, 0.0f }, angles.pitch);
		transformation.rotation *= math::Quaternion({ 0.0f, 1.0f, 0.0f, 0.0f }, angles.yaw);

		if (m_rollEnabled)
		{
			transformation.rotation *= math::Quaternion({ 0.0f, 0.0f, 1.0f, 0.0f }, angles.roll);
		}

		math::normalizeQuat(transformation.rotation);

		// and keep the quaternion of length 1.0, so it represents a valid 3D rotation
		// Also we can do it only from time to time, like every 100th call of rotating functions.
	}

	void Camera::addRelativeAngles(const Angles& angles)
	{
		m_updatedBasis = false;
		m_updatedMatrices = false;

		if (m_rollEnabled)
		{
			transformation.rotation *= math::Quaternion(right(), angles.pitch);
			transformation.rotation *= math::Quaternion(top(), angles.yaw);
			transformation.rotation *= math::Quaternion(forward(), angles.roll);
		}
		else
		{
			transformation.rotation *= math::Quaternion(right(), angles.pitch);
			transformation.rotation *= math::Quaternion({ 0.0f, 1.0f, 0.0f, 0.0f }, angles.yaw);
		}

		math::normalizeQuat(transformation.rotation);

		// optional, to remove accumulating floating point errors
		// and keep the quaternion of length 1.0, so it represents a valid 3D rotation
		// Also we can do it only from time to time, like every 100th call of rotating functions.
	}

	void Camera::updateBasis()
	{
		if (m_updatedBasis) return;
		m_updatedBasis = true;

		transformation.rotation.toMat3(m_viewInv);
	}

	void Camera::updateMatrices()
	{
		if (m_updatedMatrices) return;
		m_updatedMatrices = true;

		updateBasis();

		math::invertOrthonormal(m_viewInv, m_view);


		m_viewProj = m_view * m_proj;
		m_viewProjInv = m_projInv * m_viewInv;
	}
}