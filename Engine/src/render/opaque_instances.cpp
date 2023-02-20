#include "opaque_instances.hpp"
#include "assimpLoader.hpp"
#include "transformSystem.hpp"
#include "lightSystem.hpp"

namespace
{
	struct vertexCbuffer
	{
		XMMATRIX mat;
	};

	struct pixelCbuffer
	{
		engine::Material::Properties prop;
	};

	struct geomCbuffer
	{
		std::array<XMMATRIX, 6> viewProjMat;
		uint32_t textureCubePos;
		XMFLOAT3 padding;
	};
}

namespace engine
{
	void OpaqueInstances::init(std::shared_ptr<ShaderProgram> shader, std::shared_ptr<ShaderProgram> shaderDepthOnly)
	{
		this->shader = shader;
		vertexBufferID = shader->createUniform(sizeof(XMMATRIX), ShaderType::VERTEX);
		pixelBufferID = shader->createUniform(sizeof(Material::Properties), ShaderType::PIXEL);

		this->shaderDepthOnly = shaderDepthOnly;
		vertexBufferIDDepthOnly = shaderDepthOnly->createUniform(sizeof(XMMATRIX), ShaderType::VERTEX);
		geometryBufferIDDepthOnly = shaderDepthOnly->createUniform(sizeof(XMMATRIX) * 6u + sizeof(float) * 4, ShaderType::GEOMETRY);
	}

	uint32_t OpaqueInstances::addModel(std::shared_ptr<Model> model, XMFLOAT3 size)
	{
		perModel.push_back({ model });

		uint32_t perModelPos = perModel.size() - 1;

		for (uint32_t i = 0; i != perModel[perModelPos].model->meshes.size(); ++i)
		{
			PerMesh perMesh;
			//perMesh.transform = XMMatrixScaling(size.x, size.y, size.z);
			perModel[perModelPos].perMesh.push_back(perMesh);
		}

		return perModelPos;
	}

	uint32_t OpaqueInstances::addModel(DissolutionInstances::PerModel& dissolutionModel, uint32_t instanceID)
	{
		bool instanceFound = false;
			for (auto& mesh : dissolutionModel.perMesh)
			{
				for (auto& material : mesh.perMaterial)
				{
					for (int i = 0; i != material.instances.size(); i++)
					{
						material.instances[i].instanceID = instanceID;
					}
					if (instanceFound)
					{
						material.instances.pop_back();
						instanceFound = false;
					}
				}
			}
		perModel.push_back(dissolutionModel);
		
		return (perModel.size() - 1);
	}

	void OpaqueInstances::addMaterials(std::vector<Material>& materials, uint32_t modelPos)
	{
		for (auto& mesh : perModel[modelPos].perMesh)
		{
			mesh.perMaterial.push_back(PerMaterial());
		}

		if (materials.size() == perModel[modelPos].model->meshes.size())
		{
			for (uint32_t i = 0; i != materials.size(); ++i)
			{
				int lastID = perModel[modelPos].perMesh[i].perMaterial.size() - 1;
				perModel[modelPos].perMesh[i].perMaterial[lastID].material = materials[i];
			}
		}
	}

	void OpaqueInstances::addInstances(std::vector<Instance>& instances, uint32_t modelPos, uint32_t materialPos)
	{
		for (auto& mesh : perModel[modelPos].perMesh)
		{
			for (uint32_t i = 0; i != instances.size(); ++i)
				mesh.perMaterial[materialPos].instances.push_back(instances[i]);
		}
		updateInstanceBuffers();
	}

	void OpaqueInstances::updateInstanceBuffers()
	{
		uint32_t totalInstances = 0;
		for (auto& model : perModel)
			for (auto& perMesh : model.perMesh)
				for (const auto& material : perMesh.perMaterial)
					totalInstances += uint32_t(material.instances.size());

		if (totalInstances == 0)
			return;

		instanceBuffer.init(totalInstances * sizeof(Instance::Properties), D3D11_USAGE_DYNAMIC); // resizes if needed


		auto mapping = instanceBuffer.map();
		Instance::Properties* dst = static_cast<Instance::Properties*>(mapping.pData);

		uint32_t copiedNum = 0;
		for (const auto& model : perModel)
		{
			for (uint32_t meshIndex = 0; meshIndex < model.perMesh.size(); ++meshIndex)
			{
				const Mesh& mesh = model.model->model().meshes[meshIndex];

				for (const auto& material : model.perMesh[meshIndex].perMaterial)
				{
					auto& instances = material.instances;

					uint32_t numModelInstances = instances.size();
					for (uint32_t index = 0; index < numModelInstances; ++index)
					{
						Instance::Properties prop;
						prop.instanceID = instances[index].instanceID;
						prop.texCoordScale = instances[index].texCoordScale;
						prop.transform = TransformSystem::GetInstance()->getTransform(material.instances[index].transformID);
						dst[copiedNum++] = prop;
					}
				}
			}
		}

		instanceBuffer.unmap();
	}

	void OpaqueInstances::render(Sky& sky)
	{
				if (instanceBuffer.size() == 0)
			return;

		shader->bind();
		instanceBuffer.bind(1, sizeof(Instance::Properties));
		sky.bindIBLTextures(5);

		uint32_t renderedInstances = 0;
		s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		for (const auto& model : perModel)
		{
			if (model.empty()) continue;

			model.model->vertexBuffer().bind(0, sizeof(Mesh::Vertex));
			model.model->indexBuffer().bind();

			for (uint32_t meshIndex = 0; meshIndex < model.perMesh.size(); ++meshIndex)
			{
				const Mesh& mesh = model.model->model().meshes[meshIndex];
				const auto& meshRange = model.model->meshRange(meshIndex);

				vertexCbuffer* vbuff;
				vbuff = (vertexCbuffer*)shader->mapUniformVertex(0).pData;
				vbuff->mat = mesh.m_transform;
				shader->unmapUniformVertex(0);
				shader->bindBufferVertex(0, vertexBufferID);

				for (uint32_t materialIndex = 0; materialIndex < model.perMesh[meshIndex].perMaterial.size(); ++materialIndex)
				{
					const auto& perMaterial = model.perMesh[meshIndex].perMaterial[materialIndex];
					const auto& material = perMaterial.material;
					if (perMaterial.instances.empty()) continue;

					pixelCbuffer* pbuff;
					pbuff = (pixelCbuffer*)shader->mapUniformPixel(0).pData;
					pbuff->prop = material.properties;
					shader->unmapUniformPixel(0);
					shader->bindBufferPixel(0, pixelBufferID);

					if (material.properties.hasDiffuseTexture)
						material.diffuseTexture->bind(0);

					if (material.properties.hasNormalTexture)
						material.normalMapTexture->bind(1);

					if (material.properties.hasRoughnessTexture)
						material.roughnessTexture->bind(2);

					if (material.properties.hasMetallicTexture)
						material.metallicTexture->bind(3);

					uint32_t numInstances = uint32_t(perMaterial.instances.size());

					if (mesh.indexed)
						s_devcon->DrawIndexedInstanced(meshRange.indexNum, numInstances, meshRange.indexOffset, meshRange.vertexOffset, renderedInstances);
					else
						s_devcon->DrawInstanced(meshRange.vertexNum, numInstances, meshRange.vertexOffset, renderedInstances);
					
					renderedInstances += numInstances;
				}
			}
		}
	}

	void OpaqueInstances::renderDepthOnly()
	{
		shaderDepthOnly->bind();

		if (instanceBuffer.size() == 0)
			return;

		instanceBuffer.bind(1, sizeof(Instance::Properties));

		s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		for (int i = 0; i != LightSystem::GetInstance()->m_pointLights.size(); i++)
		{
			uint32_t renderedInstances = 0;


			geomCbuffer* data;
			
			data = (geomCbuffer*)shaderDepthOnly->mapUniformGeom(0).pData;
			auto& pointLight = LightSystem::GetInstance()->m_pointLights.at(i);
			data->viewProjMat = TransformSystem::GetInstance()->getPointLightViewProjMat(pointLight.viewMatId);
			data->textureCubePos = i;

			shaderDepthOnly->unmapUniformGeom(0);
			shaderDepthOnly->bindBufferGeom(0, this->geometryBufferIDDepthOnly);

			for (const auto& model : perModel)
			{
				model.model->vertexBuffer().bind(0, sizeof(Mesh::Vertex));
				if (model.empty()) continue;

				model.model->vertexBuffer().bind(0, sizeof(Mesh::Vertex));
				model.model->indexBuffer().bind();

				for (uint32_t meshIndex = 0; meshIndex < model.perMesh.size(); ++meshIndex)
				{
					const Mesh& mesh = model.model->model().meshes[meshIndex];
					const auto& meshRange = model.model->meshRange(meshIndex);

					D3D11_MAPPED_SUBRESOURCE res2;

					vertexCbuffer* buffer;
					res2 = shaderDepthOnly->mapUniformVertex(0);

					buffer = (vertexCbuffer*)res2.pData;
					buffer->mat = mesh.m_transform;

					shaderDepthOnly->unmapUniformVertex(0);
					shaderDepthOnly->bindBufferVertex(0, this->vertexBufferIDDepthOnly);
					for (uint32_t materialIndex = 0; materialIndex < model.perMesh[meshIndex].perMaterial.size(); ++materialIndex)
					{
						const auto& perMaterial = model.perMesh[meshIndex].perMaterial[materialIndex];
						const auto& material = perMaterial.material;
						if (perMaterial.instances.empty()) continue;

						uint32_t numInstances = uint32_t(perMaterial.instances.size());

						if (mesh.indexed)
							s_devcon->DrawIndexedInstanced(meshRange.indexNum, numInstances, meshRange.indexOffset, meshRange.vertexOffset, renderedInstances);
						else
							s_devcon->DrawInstanced(meshRange.vertexNum, numInstances, meshRange.vertexOffset, renderedInstances);

						renderedInstances += numInstances;
					}
				}
			}
		}
	}

	void OpaqueInstances::clean()
	{
		shader = nullptr;
		for (auto& model : perModel)
		{
			model.model->clean();
			for (auto& mesh : model.perMesh)
			{
				for (auto& material : mesh.perMaterial)
					material.material.clean();
			}
		}

		instanceBuffer.clean();
	}

	OpaqueInstances::Instance::Instance(DissolutionInstances::Instance& disInst)
	{
		this->texCoordScale = disInst.texCoordScale;
		this->instanceID = disInst.instanceID;
		this->transformID = disInst.transformID;
	}

	OpaqueInstances::PerMaterial::PerMaterial(DissolutionInstances::PerMaterial& disMat)
	{
		this->material = disMat.material;
		for (auto& instance : disMat.instances)
		{
			this->instances.push_back(instance);
		}
	}

	OpaqueInstances::PerMesh::PerMesh(DissolutionInstances::PerMesh& disMesh)
	{
		this->transform = disMesh.transform;
		for (auto& material : disMesh.perMaterial)
		{
			this->perMaterial.push_back(material);
		}
	}

	OpaqueInstances::PerModel::PerModel(DissolutionInstances::PerModel& disModel)
	{
		this->model = disModel.model;
		for (auto& mesh : disModel.perMesh)
		{
			this->perMesh.push_back(mesh);
		}
	}
}