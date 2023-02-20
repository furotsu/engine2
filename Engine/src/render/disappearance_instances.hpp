#pragma once 

#include "model.hpp"
#include "modelManager.hpp"
#include "sky.hpp"
#include "opaque_instances.hpp"

namespace engine
{
	class DisappearanceInstances
	{
		std::shared_ptr<ShaderProgram> shader;
		std::shared_ptr<ShaderProgram> shaderDepthOnly;
		uint32_t m_instanceCount;
		uint32_t vertexBufferIDDepthOnly;
		uint32_t geometryBufferIDDepthOnly;
		uint32_t vertexBufferID;
		uint32_t pixelBufferID;
		std::shared_ptr<Texture> m_noiseTexture;

	public:

		struct Instance
		{
			struct Sphere
			{
				XMFLOAT3 pos;
				float growthSpeed;
			};
			struct Properties
			{
				XMMATRIX transform;
				float texCoordScale;
				float startTimePoint;
				Sphere sphere;
				uint16_t instanceID;
			};
			Sphere sphere;
			uint16_t instanceID;
			float texCoordScale;
			uint32_t transformID;
			float startTimePoint;
			Instance() = delete;
			Instance(OpaqueInstances::Instance& disInst, uint32_t timePoint, Instance::Sphere& sphere);
			Instance(uint32_t transformPos, float timePoint, DisappearanceInstances::Instance::Sphere sphere, float texCoordScale = 1.0f)
				: transformID(transformPos), startTimePoint(timePoint), texCoordScale(texCoordScale)
			{
			}
		};

		struct PerMaterial
		{
			std::vector<Instance> instances;
			Material material;

			PerMaterial() = default;
			PerMaterial(OpaqueInstances::PerMaterial& disMat, uint32_t timePoint, Instance::Sphere& sphere);
		};

		struct PerMesh
		{
			XMMATRIX transform;
			std::vector<PerMaterial> perMaterial;

			PerMesh() = default;
			PerMesh(OpaqueInstances::PerMesh& disMesh, uint32_t timePoint, Instance::Sphere& sphere);
		};

		struct PerModel
		{
			std::shared_ptr<Model> model;
			std::vector<PerMesh> perMesh;

			PerModel() = default;
			PerModel(OpaqueInstances::PerModel& disModel, uint32_t timePoint, Instance::Sphere& sphere);

			bool empty() const { return model == nullptr; }

		};

		VertexBuffer<Instance> instanceBuffer;
		std::vector<PerModel> perModel;

		DisappearanceInstances() = default;
		void init(std::shared_ptr<ShaderProgram> shader, std::shared_ptr<ShaderProgram> shaderDepthOnly);
		void setNoiseTexture(std::shared_ptr<Texture> noiseTexture);

		void addModel(OpaqueInstances::PerModel& opaqueModel, uint32_t instanceID, uint32_t timePoint, Instance::Sphere& sphere);
		void addMaterials(std::vector<Material>& materials, uint32_t modelPos = 0u);
		void addInstances(std::vector<Instance>& instances, uint32_t modelPos, uint32_t materialPos);
		void updateInstanceBuffers();
		void render(Sky& sky);
		void spawnParticles(std::shared_ptr<ShaderProgram> shader);
		void renderDepthOnly();

		void clean();

		friend void loadModel(std::string path, std::shared_ptr<engine::Model> model);

	};

}