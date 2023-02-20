#define _USE_MATH_DEFINES

#include "utility.hpp"

#include <cmath>
#include <algorithm>

float math::clamp(float x, float lower, float upper)
{
    return std::min(upper, std::max(x, lower));
}

XMVECTOR math::clamp3(const XMVECTOR& vec, float lower, float upper)
{
    float x = std::min(upper, std::max(XMVectorGetX(vec), lower));
    float y = std::min(upper, std::max(XMVectorGetY(vec), lower));
    float z = std::min(upper, std::max(XMVectorGetZ(vec), lower));

    return XMVectorSet(x, y, z, XMVectorGetW(vec));
}

XMVECTOR math::lerp(const XMVECTOR& vec1, const XMVECTOR& vec2, float t)
{
    return vec1 + XMVectorScale(vec2 - vec1, t);
}

float math::lerp(float f1, const float f2, float t)
{
    return f1 + t*(f2 - f1);
}

float math::smoothstep(float edge0, float edge1,  float x)
{
    x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3 - 2 * x);
}

bool math::almostEqual(float a, float b, float epsilon)
{
    return fabs(a - b) < epsilon;
}

XMFLOAT3 math::minVec3(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
    return XMFLOAT3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}

XMFLOAT3 math::maxVec3(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
    return XMFLOAT3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}

float math::lengthVec3(const XMFLOAT3& v) { return XMVectorGetX(XMVector3Length(XMLoadFloat3(&v))); }

float math::lengthPow2Vec3(const XMFLOAT3& v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

XMFLOAT3 math::crossVec3(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
    return XMFLOAT3(v1.y * v2.z - v1.z * v2.y,
                    v1.z * v2.x - v1.x * v2.z,
                    v1.x * v2.y - v1.y * v2.x);
}

float math::dotVec3(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

XMFLOAT3 math::transformVec3(const XMFLOAT3& v, const XMMATRIX& matrix)
{
    XMVECTOR resVec = XMVector3Transform(XMLoadFloat3(&v), matrix);
    return XMFLOAT3(XMVectorGetX(resVec), XMVectorGetY(resVec), XMVectorGetZ(resVec));
}

XMFLOAT3 math::normalizeVec3(const XMFLOAT3& v)
{
    return v / lengthVec3(v);
}

