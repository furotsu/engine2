#pragma once

#include <DirectXMath.h>
#include <iostream>

using namespace DirectX;

namespace math
{
	class Quaternion;

	void normalizeQuat(Quaternion& q);

	class Quaternion
	{
		XMVECTOR m_quaternion;

	public:
		Quaternion() = default;
		Quaternion(float x, float y, float z, float w);
		Quaternion(float yaw, float pitch, float roll);
		Quaternion(XMVECTOR v);
		Quaternion(XMVECTOR axis, float angle);

		void toMat3(XMMATRIX& matrix);


		inline Quaternion& operator*=(const Quaternion& q2) { m_quaternion = XMQuaternionMultiply(m_quaternion, q2.m_quaternion); return *this; }

		friend void normalizeQuat(Quaternion& q);
		inline XMVECTOR getQuat() const { return m_quaternion; }
	};
}

