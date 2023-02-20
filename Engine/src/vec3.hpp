#pragma once

#include <DirectXMath.h>
#include <iostream>

using namespace DirectX;

XMFLOAT3 operator+(const XMFLOAT3& l, const XMFLOAT3& r);

XMFLOAT3 operator-(const XMFLOAT3& l, const XMFLOAT3& r);

XMFLOAT3 operator-(const XMFLOAT3& l);

XMFLOAT3 operator+(const XMFLOAT3& l, float r);

XMFLOAT3 operator*(const XMFLOAT3& l, float r);

XMFLOAT3 operator*(float r, const XMFLOAT3& l);

XMFLOAT3 operator/(const XMFLOAT3& l, float r);

std::ostream& operator<<(std::ostream& os, const XMFLOAT3& v);

float& getElem(XMFLOAT3& vec, uint8_t i);
