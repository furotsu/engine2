#pragma once 

#include "model.hpp"
#include "modelManager.hpp"
#include "sky.hpp"

namespace engine
{
	class DissolutionInstances
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
				float startTimePoint;
				uint16_t instanceID;
			};
			uint16_t instanceID;
			float texCoordScale;
			uint32_t transformID;
			float startTimePoint;
			Instance() = delete;
			Instance(uint32_t transformPos, float timePoint, float texCoordScale = 1.0f)
				: transformID(transformPos), startTimePoint(timePoint), texCoordScale(texCoordScale)
			{
			}

			void setID(uint16_t id) { instanceID = id; }
		};

		struct PerMaterial
		{
			std::vector<Instance> instances;
			Material material;
		};

		struct PerMesh
		{
			XMMATRIX transform;
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

		DissolutionInstances() = default;
		void init(std::shared_ptr<ShaderProgram> shader, std::shared_ptr<ShaderProgram> shaderDepthOnly);

		uint32_t addModel(std::shared_ptr<Model> model, XMFLOAT3 size = { 1.0f, 1.0f, 1.0f });

		void addMaterials(std::vector<Material>& materials, uint32_t modelPos = 0u);
		void addInstances(std::vector<Instance>& instances, uint32_t modelPos, uint32_t materialPos);
		void updateInstanceBuffers();
		void render(Sky& sky);
		void renderDepthOnly();

		void clean();

		friend void loadModel(std::string path, std::shared_ptr<engine::Model> model);

	};

}