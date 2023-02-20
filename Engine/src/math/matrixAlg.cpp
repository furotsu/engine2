#include "matrixAlg.hpp"

void math::setRowAsVec3(XMMATRIX& dst, const XMVECTOR& v, int row)
{
	dst.r[row] = XMVectorSet(XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(dst.r[row]));
}

void math::invertOrthonormal(const XMMATRIX& src, XMMATRIX& dst)
{

	dst = XMMatrixSet
	(
		XMVectorGetX(src.r[0]), XMVectorGetX(src.r[1]), XMVectorGetX(src.r[2]), XMVectorGetW(src.r[0]),
		XMVectorGetY(src.r[0]), XMVectorGetY(src.r[1]), XMVectorGetY(src.r[2]), XMVectorGetW(src.r[1]),
		XMVectorGetZ(src.r[0]), XMVectorGetZ(src.r[1]), XMVectorGetZ(src.r[2]), XMVectorGetW(src.r[2]),
		XMVectorGetX(src.r[3]), XMVectorGetY(src.r[3]), XMVectorGetZ(src.r[3]), XMVectorGetW(src.r[3])
		);

	dst.r[3] = -XMVectorScale(dst.r[0], XMVectorGetX(src.r[3])) - XMVectorScale(dst.r[1], XMVectorGetY(src.r[3])) - XMVectorScale(dst.r[2], XMVectorGetZ(src.r[3]));

	dst.r[3] = XMVectorSet(XMVectorGetX(dst.r[3]), XMVectorGetY(dst.r[3]), XMVectorGetZ(dst.r[3]), 1.0f);
}

void math::invertOrthogonal(const XMMATRIX& src, XMMATRIX& dst)
{
	dst = XMMatrixSet
	(
		XMVectorGetX(src.r[0]), XMVectorGetX(src.r[1]), XMVectorGetX(src.r[2]), XMVectorGetW(src.r[0]),
		XMVectorGetY(src.r[0]), XMVectorGetY(src.r[1]), XMVectorGetY(src.r[2]), XMVectorGetW(src.r[1]),
		XMVectorGetZ(src.r[0]), XMVectorGetZ(src.r[1]), XMVectorGetZ(src.r[2]), XMVectorGetW(src.r[2]),
		XMVectorGetX(src.r[3]), XMVectorGetY(src.r[3]), XMVectorGetZ(src.r[3]), XMVectorGetW(src.r[3])
	);

	float lengthXYZ[3] = {XMVectorGetX(XMVector3Length(dst.r[0])), XMVectorGetX(XMVector3Length(dst.r[1])), XMVectorGetX(XMVector3Length(dst.r[2]))};


	dst.r[0] = XMVectorDivide(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), XMVectorScale(dst.r[0], lengthXYZ[0]));
	dst.r[1] = XMVectorDivide(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), XMVectorScale(dst.r[1], lengthXYZ[1]));
	dst.r[2] = XMVectorDivide(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), XMVectorScale(dst.r[2], lengthXYZ[2]));

	dst.r[3] = -XMVectorScale(XMVectorScale(dst.r[0], XMVectorGetX(src.r[0])), 1.0f/lengthXYZ[0]) \
				- XMVectorScale(XMVectorScale(dst.r[1], XMVectorGetX(src.r[1])), 1.0f / lengthXYZ[1]) \
				- XMVectorScale(XMVectorScale(dst.r[2], XMVectorGetX(src.r[2])), 1.0f / lengthXYZ[2]);

	dst.r[3] = XMVectorSet(XMVectorGetX(dst.r[3]), XMVectorGetY(dst.r[3]), XMVectorGetZ(dst.r[3]), 1.0f);
}
