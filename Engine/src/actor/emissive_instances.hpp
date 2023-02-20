#pragma once 

#include "model.hpp"
#include "modelManager.hpp"
#include "lightSource.hpp"

namespace engine
{
	class EmissiveInstances
	{
		std::shared_ptr<ShaderProgram> shader;
		uint32_t m_instanceCount;
	public:

		struct Instance
		{
			struct Properties
			{
				XMFLOAT3 irradiance;
				float padding;
			};
			uint32_t transformID;
			uint32_t lightSourceIndex;
			LightType type;

			Instance() = delete;
			Instance(uint32_t transformPos) { transformID = transformPos; }
		};

		struct PerMaterial
		{
			std::vector<Instance> instances;
			Material material;
		};

		struct PerMesh
		{
			std::vector<PerMaterial> perMaterial;
		};

		struct PerModel
		{
			std::shared_ptr<Model> model;
			std::vector<PerMesh> perMesh;

			bool empty() const { return model == nullptr; }

		};

		VertexBuffer<Instance> instanceBuffer;
		std::vector<PerModel> perModel;

		EmissiveInstances() = default;
		void init(std::shared_ptr<ShaderProgram> shader);

		uint32_t addModel(XMFLOAT3 size = { 1.0f, 1.0f, 1.0f });
		void addMaterials(std::vector<Material>& materials, uint32_t modelPos = 0u);
		void addInstances(std::vector<Instance>& instances, uint32_t modelPos, uint32_t materialPos);
		void updateInstanceBuffers();
		void render();

		void clean();

		friend void loadModel(std::string path, std::shared_ptr<engine::Model> model);

	};

}