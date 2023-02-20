#include "sphere.hpp"

namespace math
{
	bool Sphere::hit(const ray& r, Intersection& rec, float tMin, float tMax) const
	{
		XMVECTOR ac = r.origin - center;
		//We can get rid of "a" variable because of the assumption that r direction is normalized
		float b = XMVectorGetX(XMVector3Dot(r.direction, ac)); // we can cut it in advance to avoid excess operations with discriminant after
		float c = XMVectorGetX(XMVector3Dot(ac, ac)) - radius * radius;
		float discriminant = b * b - c; // we take out 2 out of sqrt(discriminant) later to cut it with 2 in denominator
		if (discriminant >= 0)
		{
			discriminant = sqrt(discriminant);
			float hitParam = (-b - discriminant);

			if (hitParam >= rec.hitParam)
				return false;

			if (hitParam > tMax || hitParam < tMin)
			{
				return false;
			}
			
			rec.hitParam = hitParam;
			rec.point = r.pointAtParameter(hitParam);
			rec.normal = ((rec.point - this->center) / radius);
			return true;
			
		}
		return false;
	}
}