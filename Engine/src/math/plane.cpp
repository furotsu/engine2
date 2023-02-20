#include "plane.hpp"

#include <iostream>

math::Plane::Plane(XMVECTOR normal, XMVECTOR centerPos)
    : m_normal(XMVector3Normalize(normal)), m_center(centerPos)
{
}

math::Plane& math::Plane::operator=(const Plane& p2)
{
    m_normal = p2.m_normal;
    m_center = p2.m_center;
    return *this;
}

bool math::Plane::hit(const ray& r, Intersection& hr)
{
    float denom = XMVectorGetX(XMVector3Dot(m_normal, XMLoadFloat3(&r.direction)));
    if (abs(denom) > 0.0001f)
    {
        float hitParam = XMVectorGetX(XMVector3Dot(m_center - XMLoadFloat3(&r.origin), m_normal)) / denom;
        if (hitParam >= hr.hitParam)
        { 
            return false;
        }
        if (hitParam > 0)
        {

            hr.hitParam = hitParam;
            hr.normal = m_normal;
            XMFLOAT3 p = r.origin + hitParam * r.direction;
            hr.point = XMLoadFloat3(&(p));
            return true;
        }
    }
    return false;
}
