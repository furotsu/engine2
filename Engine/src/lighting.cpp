#include "scene.hpp"

XMVECTOR acesHdr2Ldr(const XMVECTOR& hdr)
{
	XMMATRIX m1 = XMMatrixSet(
		0.59719f, 0.07600f, 0.02840f, 0.0f,
		0.35458f, 0.90834f, 0.13383f, 0.0f,
		0.04823f, 0.01566f, 0.83777f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	);
	XMMATRIX m2 = XMMatrixSet(
		1.60475f, -0.10208, -0.00327f, 0.0f,
		-0.53108f, 1.10813, -0.07276f, 0.0f,
		-0.07367f, -0.00605, 1.07602f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	);
	XMVECTOR v = XMVector3Transform(hdr, m1);
	XMVECTOR a = v * (v + XMVectorSet(0.0245786f, 0.0245786f, 0.0245786f, 0.0f)) - XMVectorSet(0.000090537f, 0.000090537f, 0.000090537f, 0.0f);
	XMVECTOR b = v * (XMVectorSet(0.983729f, 0.983729f, 0.983729f, 0.0f) * v + XMVectorSet(0.4329510f, 0.4329510f, 0.4329510f, 0.0f)) + XMVectorSet(0.238081f, 0.238081f, 0.238081f, 0.0f);
	XMVECTOR ldr = math::clamp3(XMVector3Transform(a / b, m2), 0.0f, 1.0f);
	return ldr;
}

XMVECTOR adjustExposure(const XMVECTOR& color, float EV100)
{
	float LMax = (78.0f / (0.65f * 100.0f)) * powf(2.0f, EV100);
	return color * (1.0f / LMax);
}

XMVECTOR correctGamma(const XMVECTOR& color, float gamma)
{
	XMVECTOR g = XMVectorSet(gamma, gamma, gamma, 0.0f);
	return XMVectorPow(color, XMVectorDivide(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), g));
}

void branchlessONB(const XMVECTOR& n, XMVECTOR& b1, XMVECTOR& b2)
{
	float sign = copysignf(1.0f, XMVectorGetZ(n));
	const float a = -1.0f / (sign + XMVectorGetZ(n));
	const float b = XMVectorGetX(n) * XMVectorGetY(n) * a;
	b1 = XMVectorSet(1.0f + sign * XMVectorGetX(n) * XMVectorGetX(n) * a, sign * b, -sign * XMVectorGetX(n), 0.0f);
	b2 = XMVectorSet(b, sign + XMVectorGetY(n) * XMVectorGetY(n) * a, -XMVectorGetY(n), 0.0f);
}

void fibonacciHemisphereDirection(uint32_t i, XMVECTOR& direction)
{
	float theta = 2.0f * M_PI * i / GOLDEN_RATIO;
	float phi = std::acosf(1.0f - 2.0f * (float(i) + 0.5f) / static_cast<float>(RAYS_ABOVE_HEMISPHERE_COUNT));

	float x = std::cosf(theta) * std::sinf(phi);
	float y = std::sinf(theta) * std::sinf(phi);
	float z = -std::cosf(phi);
	z = (z < 0.0f) ? -z : z;
	direction = XMVectorSet(x, y, z, 0.0f);
}

// Schlick's approximation of Fresnel reflectance
XMVECTOR frensel(float NdotL, const XMVECTOR& F0)
{
	return F0 + (XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f) - F0) * powf(1 - NdotL, 5.0);
}

// Height-correlated Smith G2 for GGX,
// Filament, 4.4.2 Geometric shadowing
float smith(float rough2, float NoV, float NoL)
{
	NoV *= NoV;
	NoL *= NoL;
	return 2.0f / (sqrtf(1.0f + rough2 * (1 - NoV) / NoV) + sqrtf(1.0f + rough2 * (1 - NoL) / NoL));
}

// GGX normal distribution,
// Real-Time Rendering 4th Edition, page 340, equation 9.41
float ggx(float rough2, float NoH, float LightAngleSin, float lightAngleCos)
{
	float denom = NoH * NoH * (rough2 - 1.0f) + 1.0f;
	denom = M_PI * denom * denom;
	return rough2 / denom;
}
