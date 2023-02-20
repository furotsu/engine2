#include <iostream>

#include "triangle.hpp"
using namespace DirectX;

bool math::Triangle::hit(const ray& r, Intersection& rec, float tMin, float tMax) const
{
	float NdotRayDirection = XMVectorGetX(XMVector3Dot(normal, r.direction));

	if (fabs(NdotRayDirection) < 0.01f)
		return false; // paralel

	float D = -XMVectorGetX(XMVector3Dot(normal, vertices[0]));

	float hitParam = -(XMVectorGetX(XMVector3Dot(normal, r.origin)) + D) / NdotRayDirection;


	if (hitParam >= rec.hitParam)
		return false;

	if (hitParam > tMax || hitParam < tMin)
		return false;

	XMVECTOR point = r.origin + hitParam * r.direction;	


	XMVECTOR edge = vertices[1] - vertices[0];
	XMVECTOR c = point - vertices[0];

	if (XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(edge, c))) < 0) return false;
	edge = vertices[2] - vertices[1];
	c = point - vertices[1];

	if (XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(edge, c))) < 0) return false;

	edge = vertices[0] - vertices[2];
	c = point - vertices[2];
	if (XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(edge, c))) < 0) return false;


	rec.hitParam = hitParam;
	rec.point = point;
	rec.normal = normal;
	return true;

}
