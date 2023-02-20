#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "model.hpp"
#include "textureManager.hpp"

namespace assimp
{
	void loadModel(std::string path, std::string filename, std::shared_ptr<engine::Model> model);
}