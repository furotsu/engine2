#include "emissive_instances.hpp"
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
		uint32_t renderedInstances;
	};
}

namespace engine
{

	void EmissiveInstances::init(std::shared_ptr<ShaderProgram> shader)
	{
		this->shader = shader;
		shader->createUniform(sizeof(XMMATRIX), ShaderType::VERTEX);
		shader->createUniform(sizeof(float) * 4, ShaderType::PIXEL);
	}

	uint32_t EmissiveInstances::addModel(XMFLOAT3 size)
	{
		std::shared_ptr<Model> model = ModelManager::GetInstance()->initUnitSphereFlat();
		perModel.push_back({ model });

		uint32_t perModelPos = perModel.size() - 1;

		for (uint32_t i = 0; i != perModel[perModelPos].model->meshes.size(); ++i)
		{
			perModel[perModelPos].perMesh.push_back(PerMesh());

			perModel[perModelPos].model->meshes[i].m_transform = XMMatrixMultiply(perModel[perModelPos].model->meshes[i].m_transform, XMMatrixScaling(size.x, size.y, size.z));
		}

		return perModelPos;
	}

	void EmissiveInstances::addMaterials(std::vector<Material>& materials, uint32_t modelPos)
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

	void EmissiveInstances::addInstances(std::vector<Instance>& instances, uint32_t modelPos, uint32_t materialPos)
	{
		for (auto& mesh : perModel[modelPos].perMesh)
		{
			for (uint32_t i = 0; i != instances.size(); ++i)
				mesh.perMaterial[materialPos].instances.push_back(instances[i]);
		}
		updateInstanceBuffers();
	}

	void EmissiveInstances::updateInstanceBuffers()
	{
		uint32_t totalInstances = 0;
		for (auto& model : perModel)
			for (auto& perMesh : model.perMesh)
				for (const auto& material : perMesh.perMaterial)
					totalInstances += uint32_t(material.instances.size());

		if (totalInstances == 0)
			return;

		instanceBuffer.init(totalInstances * sizeof(XMMATRIX), D3D11_USAGE_DYNAMIC); // resizes if needed

		auto mapping = instanceBuffer.map();
		XMMATRIX* dst = static_cast<XMMATRIX*>(mapping.pData);

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
						XMMATRIX instanceTransform = TransformSystem::GetInstance()->getTransform(material.instances[index].transformID);
						dst[copiedNum++] = instanceTransform;;
					}
				}
			}
		}

		instanceBuffer.unmap();
	}

	void EmissiveInstances::render()
	{
		if (instanceBuffer.size() == 0)
			return;

		shader->bind();
		instanceBuffer.bind(1, sizeof(XMMATRIX));

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
				shader->bindBufferVertex(0, 0);



				for (uint32_t materialIndex = 0; materialIndex < model.perMesh[meshIndex].perMaterial.size(); ++materialIndex)
				{
					const auto& perMaterial = model.perMesh[meshIndex].perMaterial[materialIndex];
					const auto& material = perMaterial.material;
					
					if (perMaterial.instances.empty()) continue;

					uint32_t numInstances = uint32_t(perMaterial.instances.size());
					
					pixelCbuffer* pbuff;

					pbuff = (pixelCbuffer*)shader->mapUniformPixel(0).pData;
					pbuff->renderedInstances = renderedInstances;
					shader->unmapUniformPixel(0);
					shader->bindBufferPixel(0, 0);

					if (mesh.indexed)
						s_devcon->DrawIndexedInstanced(meshRange.indexNum, numInstances, meshRange.indexOffset, meshRange.vertexOffset, renderedInstances);
					else
						s_devcon->DrawInstanced(meshRange.vertexNum, numInstances, meshRange.vertexOffset, renderedInstances);
					
					renderedInstances += numInstances;
				}
			}
		}
	}
	void EmissiveInstances::clean()
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
}