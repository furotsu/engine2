#pragma once

#include <DirectXMath.h>

#include "texture.hpp"
#include "DxRes.hpp"

using namespace DirectX;

namespace engine
{
	struct Grass
	{
		XMFLOAT3 position;
		XMFLOAT2 size;

		std::shared_ptr<Texture> albedoTexture;
		std::shared_ptr<Texture> normalMapTexture;
		std::shared_ptr<Texture> roughnessTexture;
		std::shared_ptr<Texture> opacityTexture;
	};
}