#pragma once
#include <DirectXMath.h>
#include "constants.hpp"
#include "vec3.hpp"
using namespace DirectX;

namespace math
{
	float clamp(float x, float lower, float upper);

	XMVECTOR clamp3(const XMVECTOR& vec, float lower, float upper);
	XMVECTOR lerp(const XMVECTOR& vec1, const XMVECTOR& vec2, float t);
	float lerp(float f1, const float f2, float t);

	float smoothstep(float edge0, float edge1, float x);

	bool almostEqual(float a, float b, float epsilon = FLOAT_COMPARE_EPSILON);

	XMFLOAT3 minVec3(const XMFLOAT3& v1, const XMFLOAT3& v2);
	XMFLOAT3 maxVec3(const XMFLOAT3& v1, const XMFLOAT3& v2);
	float lengthVec3(const XMFLOAT3& v);
	float lengthPow2Vec3(const XMFLOAT3& v);
	XMFLOAT3 normalizeVec3(const XMFLOAT3& v);
	XMFLOAT3 crossVec3(const XMFLOAT3& v1, const XMFLOAT3& v2);
	float dotVec3(const XMFLOAT3& v1, const XMFLOAT3& v2);
	XMFLOAT3 transformVec3(const XMFLOAT3& v, const XMMATRIX& matrix);
}