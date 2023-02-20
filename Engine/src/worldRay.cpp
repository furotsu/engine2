#include "worldRay.hpp"

#include <algorithm>

#include <iostream>
bool engine::Ray::intersect(float boxT, const Box& box) const
{
	float tmin = (box.min.x - r.origin.x) / r.direction.x;
	float tmax = (box.max.x - r.origin.x) / r.direction.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (box.min.y - r.origin.y) / r.direction.y;
	float tymax = (box.max.y - r.origin.y) / r.direction.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (box.min.z - r.origin.z) / r.direction.z;
	float tzmax = (box.max.z - r.origin.z) / r.direction.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

bool engine::Ray::intersectSphere(MeshIntersection& intersection, const Box& box) const
{
	float radius = box.radius();
	XMFLOAT3 ac = r.origin - box.center();
	//We can get rid of "a" variable because of the assumption that r direction is normalized
	float b = math::dotVec3(r.direction, ac); // we can cut it in advance to avoid excess operations with discriminant after
	float c = math::dotVec3(ac, ac) - radius * radius;
	float discriminant = b * b - c; // we take out 2 out of sqrt(discriminant) later to cut it with 2 in denominator
	if (discriminant >= 0)
	{
		discriminant = sqrt(discriminant);
		float hitParam = (-b - discriminant);

		if (hitParam >= intersection.t)
			return false;

		if (hitParam > RAY_MAX || hitParam < RAY_MIN)
		{
			return false;
		}

		intersection.t = hitParam;
		intersection.pos = r.pointAtParameter(hitParam);
		intersection.normal = ((intersection.pos- box.center()) / radius);
		return true;

	}
	return false;
}

bool engine::Ray::intersect(MeshIntersection& intersection, const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3) const
{
	XMVECTOR V1 = XMLoadFloat3(&v1);
	XMVECTOR V2 = XMLoadFloat3(&v2);
	XMVECTOR V3 = XMLoadFloat3(&v3);

	XMFLOAT3 t1 = v2 - v1;
	XMFLOAT3 t2 = v3 - v1;
	XMVECTOR normal = XMVector3Cross(XMLoadFloat3(&(t1)), XMLoadFloat3(&(t2)));

	float NdotRayDirection = XMVectorGetX(XMVector3Dot(normal, XMLoadFloat3(&this->r.direction)));

	if (fabs(NdotRayDirection) < SMALL_OFFSET)
		return false; // paralel

	float D = -XMVectorGetX(XMVector3Dot(normal, V1));

	float hitParam = -(XMVectorGetX(XMVector3Dot(normal, XMLoadFloat3(&r.origin))) + D) / NdotRayDirection;

	if (hitParam >= intersection.t)
		return false;
	
	if (hitParam > RAY_MAX || hitParam < intersection.nearP)
		return false;

	XMVECTOR point = XMLoadFloat3(&r.origin) + XMVectorScale(XMLoadFloat3(&r.direction), hitParam);

	XMVECTOR edge = V2 - V1;
	XMVECTOR c = point - V1;

	if (XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(edge, c))) < 0) return false;
	edge = V3 - V2;
	c = point - V2;

	if (XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(edge, c))) < 0) return false;

	edge = V1 - V3;
	c = point - V3;
	if (XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(edge, c))) < 0) return false;

	intersection.t = hitParam;
	intersection.pos = XMFLOAT3(XMVectorGetX(point), XMVectorGetY(point), XMVectorGetZ(point));
	intersection.normal = XMFLOAT3(XMVectorGetX(normal), XMVectorGetY(normal), XMVectorGetZ(normal));
	return true;
}
