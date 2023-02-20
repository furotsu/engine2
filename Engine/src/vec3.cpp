#include "vec3.hpp"
#include "debug.hpp"

XMFLOAT3 operator+(const XMFLOAT3& l, const XMFLOAT3& r)
{
    return XMFLOAT3(l.x + r.x, l.y + r.y, l.z + r.z);
}

XMFLOAT3 operator-(const XMFLOAT3& l, const XMFLOAT3& r)
{
    return XMFLOAT3(l.x - r.x, l.y - r.y, l.z - r.z);
}

XMFLOAT3 operator-(const XMFLOAT3& l)
{
    return XMFLOAT3(-l.x, -l.y, -l.z);
}

XMFLOAT3 operator+(const XMFLOAT3& l, float r)
{
    return XMFLOAT3(l.x + r, l.y + r, l.z + r);
}

XMFLOAT3 operator*(const XMFLOAT3& l, float r)
{
    return XMFLOAT3(l.x * r, l.y * r, l.z * r);
}

XMFLOAT3 operator*(float r, const XMFLOAT3& l)
{
    return XMFLOAT3(l.x * r, l.y * r, l.z * r);
}

XMFLOAT3 operator/(const XMFLOAT3& l, float r)
{
    return XMFLOAT3(l.x / r, l.y / r, l.z / r);
}

std::ostream& operator<<(std::ostream& os, const XMFLOAT3& v)
{
    os << v.x << " " << v.y << " " << v.z;
    return os;
}

float& getElem(XMFLOAT3& vec, uint8_t i)
{
    ASSERT(i >= 0 && i < 3);
    if (i)
    {
        if (i == 2)
            return vec.z;
        return vec.y;
    }
    return vec.x;
}
