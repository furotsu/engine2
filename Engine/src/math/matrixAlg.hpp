#pragma once

#include <DirectXMath.h>
using namespace DirectX;
namespace math
{
	void invertOrthonormal(const XMMATRIX& src, XMMATRIX& dst);

	void invertOrthogonal(const XMMATRIX& src, XMMATRIX& dst);

	inline void setRowAsVec3(XMMATRIX&, const XMVECTOR&, int);
}
