#pragma once

#include <memory>
#include <map>

#include "DxRes.hpp"
#include "opaque_instances.hpp"
#include "dissolution_instances.hpp"
#include "disappearance_instances.hpp"
#include "emissive_instances.hpp"
#include "grassField.hpp"
#include "triangleOctree.hpp"
#include "solidVector.hpp"
#include "camera.hpp"
#include "sky.hpp"

namespace engine
{
	class MeshSystem
	{
	protected:
		static MeshSystem* s_manager;

		OpaqueInstances m_opaqueInstances;
		DissolutionInstances m_dissolutionInstances;
		DisappearanceInstances m_disappearanceInstances;

		EmissiveInstances m_emissiveInstances;
		uint32_t m_terrainID;
		uint16_t m_lastID = GRASS_ID + 1;

	public:
		MeshSystem();
		MeshSystem(MeshSystem& other) = delete;
		void operator=(const MeshSystem&) = delete;

		static void init();
		static void deinit();
		static MeshSystem* GetInstance();

		void addOpaqueModel(std::shared_ptr<Model> model,std::vector<std::vector<OpaqueInstances::Instance>> instances, std::vector<Material>& materials);
		void addOpaqueModel(DissolutionInstances::PerModel model);
		OpaqueInstances::PerModel getOpaqueInstance(uint32_t instanceID);
		void deleteOpaqueInstance(uint32_t instanceID);
		
		uint32_t addDissolutionModel(std::shared_ptr<Model> model, std::vector<std::vector<DissolutionInstances::Instance>> instances, std::vector<Material>& materials);
		DissolutionInstances::PerModel getDissolutionInstance(uint32_t dissolutionModelPos);
		void deleteDissolutionInstance(uint32_t dissolutionModelPos);
		
		uint32_t addDisappearanceModel(OpaqueInstances::PerModel model, uint32_t timePoint, DisappearanceInstances::Instance::Sphere sphere);
		void deleteDisappearanceModel(uint32_t disappearanceModelID);

		void addEmissiveModel(std::vector<EmissiveInstances::Instance> instances);

		inline void setTerrainID(uint32_t id) { m_terrainID = id; }

		void updateOpaqueInstanceBuffer();
		void updateEmissiveInstanceBuffer();

		void render(Sky& sky);
		void renderEmissive();
		void renderDissolution(Sky& sky);
		void renderDisappearance(Sky& sky);
		void spawnDisappearanceParticles(std::shared_ptr<ShaderProgram> shader);
		void renderDepthOnly(const Camera& camera);

		bool intersects(Ray& r, MeshIntersection& intersection);
		
		void clean();

		uint16_t generateID();
	};
}