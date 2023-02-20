#pragma once 

#include "model.hpp"
#include "modelManager.hpp"
#include "sky.hpp"
#include "dissolution_instances.hpp"

namespace engine
{
	class OpaqueInstances
	{
		std::shared_ptr<ShaderProgram> shader;
		std::shared_ptr<ShaderProgram> shaderDepthOnly;
		uint32_t m_instanceCount;
		uint32_t vertexBufferIDDepthOnly;
		uint32_t geometryBufferIDDepthOnly;
		uint32_t vertexBufferID;
		uint32_t pixelBufferID;

	public:
		
		struct Instance
		{
			struct Properties
			{
				XMMATRIX transform;
				float texCoordScale;
				uint16_t instanceID;
			};
			uint16_t instanceID;
			float texCoordScale;
			uint32_t transformID;
			Instance() = delete;
			Instance(DissolutionInstances::Instance& disInst);
			Instance(uint32_t transformPos, float texCoordScale = 1.0f)
				: transformID(transformPos), texCoordScale(texCoordScale)
			{
			}

			void setID(uint16_t id) { instanceID = id; }
		};

		struct PerMaterial
		{
			std::vector<Instance> instances;
			Material material;

			PerMaterial() = default;
			PerMaterial(DissolutionInstances::PerMaterial& disMat);
		};

		struct PerMesh
		{
			XMMATRIX transform;
			std::vector<PerMaterial> perMaterial;

			PerMesh() = default;
			PerMesh(DissolutionInstances::PerMesh& disMesh);
		};

		struct PerModel
		{
			std::shared_ptr<Model> model;
			std::vector<PerMesh> perMesh;

			PerModel() = default;
			PerModel(std::shared_ptr<Model> model) { this->model = model; }
			PerModel(DissolutionInstances::PerModel& disModel);

			bool empty() const { return model == nullptr; }

		};

		VertexBuffer<Instance> instanceBuffer;
		std::vector<PerModel> perModel;

		OpaqueInstances() = default;
		void init(std::shared_ptr<ShaderProgram> shader, std::shared_ptr<ShaderProgram> shaderDepthOnly);

		uint32_t addModel(std::shared_ptr<Model> model, XMFLOAT3 size = { 1.0f, 1.0f, 1.0f });
		uint32_t addModel(DissolutionInstances::PerModel& dissolutionModel, uint32_t instanceID);
		void addMaterials(std::vector<Material>& materials, uint32_t modelPos = 0u);
		void addInstances(std::vector<Instance>& instances, uint32_t modelPos, uint32_t materialPos);
		void updateInstanceBuffers();
		void render(Sky& sky);
		void renderDepthOnly();

		void clean();

		friend void loadModel(std::string path, std::shared_ptr<engine::Model> model);

	};

}