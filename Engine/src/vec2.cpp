#include "vec3.hpp"
#include "debug.hpp"

XMFLOAT2 operator+(const XMFLOAT2& l, float r)
{
    return XMFLOAT2(l.x + r, l.y + r);
}