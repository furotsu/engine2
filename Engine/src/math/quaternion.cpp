#include "quaternion.hpp"

math::Quaternion::Quaternion(float x, float y, float z, float w)
	: m_quaternion(XMVectorSet(x, y, z, w))
{
}

math::Quaternion::Quaternion(float yaw, float pitch, float roll)
{
	m_quaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll));
}

math::Quaternion::Quaternion(XMVECTOR v)
	: m_quaternion(v)
{
}

math::Quaternion::Quaternion(XMVECTOR axis, float angle)
{
	m_quaternion = XMQuaternionRotationAxis(axis, XMConvertToRadians(angle));
}

void math::Quaternion::toMat3(XMMATRIX& matrix)
{
	float x = XMVectorGetX(m_quaternion);
	float y = XMVectorGetY(m_quaternion);
	float z = XMVectorGetZ(m_quaternion);
	float w = XMVectorGetW(m_quaternion);

	matrix = XMMatrixSet(
		1.0f - 2.0f * y * y - 2.0f * z * z,  2.0f * x * y + 2.0f * z * w,		 2.0f * x * z - 2.0f * y * w,		   XMVectorGetW(matrix.r[0]),
		2.0f * x * y - 2.0f * z * w,		 1.0f - 2.0f * x * x - 2.0f * z * z, 2.0f * y * z + 2.0f * x * w,		   XMVectorGetW(matrix.r[1]),
		2.0f * x * z + 2.0f * y * w,		 2.0f * y * z - 2.0f * x * w,		 1.0f - 2.0f * x * x - 2.0f * y * y,   XMVectorGetW(matrix.r[2]),
		XMVectorGetX(matrix.r[3]),			 XMVectorGetY(matrix.r[3]),			 XMVectorGetZ(matrix.r[3]),			   XMVectorGetW(matrix.r[3])
	);	
}

void math::normalizeQuat(Quaternion& q)
{
	q.m_quaternion = XMQuaternionNormalize(q.m_quaternion);
}

