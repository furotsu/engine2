#include "plane.hpp"

#include <iostream>

math::Plane::Plane(XMFLOAT3 normal, XMFLOAT3 centerPos)
    : m_normal(math::normalizeVec3(normal)), m_center(centerPos)
{
}

math::Plane& math::Plane::operator=(const Plane& p2)
{
    m_normal = p2.m_normal;
    m_center = p2.m_center;
    return *this;
}

void math::Plane::setNormal(const XMFLOAT3& normal)
{
    m_normal = math::normalizeVec3(normal);
}

void math::Plane::setNormal(const XMVECTOR& normal)
{
    XMFLOAT3 normalVec3 = math::normalizeVec3(XMFLOAT3(XMVectorGetX(normal), XMVectorGetY(normal), XMVectorGetZ(normal)));
    m_normal = normalVec3;
}

void math::Plane::moveCenter(const XMVECTOR& offset)
{
    XMFLOAT3 o = XMFLOAT3(XMVectorGetX(offset), XMVectorGetY(offset), XMVectorGetZ(offset));
    m_center = m_center + o;
}

void math::Plane::setCenter(const XMFLOAT3& position)
{
    m_center = position;
}

bool math::Plane::hit(const ray& r, Intersection& hr)
{
    float denom = math::dotVec3(m_normal, r.direction);
    if (abs(denom) > 0.0001f)
    {
        float hitParam = math::dotVec3(m_center - r.origin, m_normal) / denom;
        if (hitParam >= hr.hitParam)
        {
            return false;
        }
        if (hitParam > 0)
        {

            hr.hitParam = hitParam;
            hr.normal = m_normal;
            hr.point = r.origin + hitParam * r.direction;
            return true;
        }
    }
    return false;
}