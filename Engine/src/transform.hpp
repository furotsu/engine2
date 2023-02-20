#pragma once

#include <memory>

#include "quaternion.hpp"
#include "material.hpp"

struct Angles
{
	float roll;
	float pitch;
	float yaw;

	Angles() = default;
	Angles(float r, float p, float y)
		: roll(r), pitch(p), yaw(y)
	{}
};

struct Transform // We can build a transformation matrix from this
{
	math::Quaternion rotation;
	XMVECTOR position;
	XMVECTOR scale;

	XMMATRIX toMat() const;

	Transform() = default;
};