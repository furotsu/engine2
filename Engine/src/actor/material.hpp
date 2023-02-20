#pragma once

#include "textureManager.hpp"

namespace engine
{
	struct Material
	{
		struct Properties
		{
			float roughness;
			float metalness;
			
			float hasDiffuseTexture;
			float hasNormalTexture;
			float hasRoughnessTexture;
			float hasMetallicTexture;
			float reverseNormalTextureY;

			float padding;
		};

		std::shared_ptr<Texture> diffuseTexture;
		std::shared_ptr<Texture> normalMapTexture;
		std::shared_ptr<Texture> roughnessTexture;
		std::shared_ptr<Texture> metallicTexture;
		std::shared_ptr<Texture> noiseTexture;
		Properties properties;


		Material() = default;
		Material(std::shared_ptr<Texture> diffuse) { diffuseTexture = diffuse; normalMapTexture = nullptr; }
		Material(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> normal) { diffuseTexture = diffuse; normalMapTexture = normal; }

		void clean() { diffuseTexture = nullptr; normalMapTexture = nullptr; }
	};

}