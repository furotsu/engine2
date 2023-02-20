#pragma once

#include <memory>
#include <map>

#include "DxRes.hpp"
#include "model.hpp"

namespace engine
{
	class ModelManager
	{
	protected:
		static ModelManager* s_manager;

		std::map<std::string, std::shared_ptr<Model>> m_Models;

	public:
		ModelManager();
		ModelManager(ModelManager& other) = delete;
		void operator=(const ModelManager&) = delete;

		static void init();
		static void deinit();
		static ModelManager* GetInstance();

		static std::shared_ptr<Model> getModel(std::string filepath, std::string filename);
		static std::shared_ptr<Model> initUnitSphereFlat();
		static std::shared_ptr<Model> initUnitCube();

		static void clean();
	};
}