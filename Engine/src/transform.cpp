#include "transform.hpp"

XMMATRIX Transform::toMat() const
{
    return XMMatrixTransformation(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), scale,
                                  XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), rotation.getQuat(), position);
}
