#include "modelManager.hpp"
#include "assimpLoader.hpp"
#include "transformSystem.hpp"

namespace engine
{
	ModelManager* ModelManager::s_manager = nullptr;

	ModelManager::ModelManager()
	{
	}

	void ModelManager::init()
	{
		if (s_manager == nullptr)
		{
			s_manager = new ModelManager();
		}
		else
		{
			ASSERT(false && "Initializing \" ModelManager \" singleton more than once ");
		}
	}

	void ModelManager::deinit()
	{
		if (s_manager == nullptr)
		{
			ASSERT(false && "Trying to delete \" ModelManager \" singleton more than once ");
		}
		else
		{
			clean();
			delete s_manager;
			s_manager = nullptr;
		}
	}

	ModelManager* ModelManager::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ASSERT(false && "Trying to call \" TextureManager \" singleton  instance before initializing it");
		}
#endif
		return s_manager;
	}

	std::shared_ptr<Model> ModelManager::getModel(std::string filepath, std::string filename)
	{
		auto it = GetInstance()->m_Models.find(filepath);

		if (it != GetInstance()->m_Models.end())
		{
			return it->second;
		}
		else
		{
			std::shared_ptr<Model> model = std::make_shared<Model>();
			assimp::loadModel(filepath, filename, model);
			model->updateTriangleOctrees();
			GetInstance()->m_Models.emplace(std::make_pair(filepath, model));
			return model;
		}
		return nullptr;
	}

	std::shared_ptr<Model> ModelManager::initUnitSphereFlat()
	{
		auto it = GetInstance()->m_Models.find("UNIT_SPHERE_FLAT");

		if (it != GetInstance()->m_Models.end())
		{
			return it->second;
		}
		else
		{ 
			const uint32_t SIDES = 6;
			const uint32_t GRID_SIZE = 12;
			const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
			const uint32_t VERT_PER_SIZE = 3 * TRIS_PER_SIDE;

			std::shared_ptr<Model> model = std::make_shared<Model>();
			model->name = "UNIT_SPHERE_FLAT";
			model->box = Box::empty();
			model->box.min = XMFLOAT3(-0.58f, -0.58f, -0.58f); // for radius to be equal to 1
			model->box.max = XMFLOAT3(0.58f, 0.58f, 0.58f);
			Model::MeshRange range;
			range.indexNum = 0;
			range.vertexNum = VERT_PER_SIZE * SIDES ;
			range.indexOffset = 0;
			range.vertexOffset = 0;
			model->addMeshRange(range);

			std::vector<Mesh> meshes;
			engine::Mesh& mesh = meshes.emplace_back();
			mesh.indexed = false;
			mesh.name = "UNIT_SPHERE_FLAT";
			mesh.box = model->box;
			mesh.m_transform = XMMatrixIdentity();
			mesh.m_transformInv = XMMatrixIdentity();
			
			mesh.vertices.resize(VERT_PER_SIZE * SIDES );
			Mesh::Vertex* vertex = mesh.vertices.data();

			int sideMasks[6][3] =
			{
				{ 2, 1, 0 },
				{ 0, 1, 2 },
				{ 2, 1, 0 },
				{ 0, 1, 2 },
				{ 0, 2, 1 },
				{ 0, 2, 1 }
			};

			float sideSigns[6][3] =
			{
				{ +1, +1, +1 },
				{ -1, +1, +1 },
				{ -1, +1, -1 },
				{ +1, +1, -1 },
				{ +1, -1, -1 },
				{ +1, +1, +1 }
			};

			for (int side = 0; side < SIDES; ++side)
			{
				for (int row = 0; row < GRID_SIZE; ++row)
				{
					for (int col = 0; col < GRID_SIZE; ++col)
					{
						float left = (col + 0) / float(GRID_SIZE) * 2.f - 1.f;
						float right = (col + 1) / float(GRID_SIZE) * 2.f - 1.f;
						float bottom = (row + 0) / float(GRID_SIZE) * 2.f - 1.f;
						float top = (row + 1) / float(GRID_SIZE) * 2.f - 1.f;

						XMFLOAT3 quad[4] =
						{
							{ left, bottom, 1.f },
							{ right, bottom, 1.f },
							{ left, top, 1.f },
							{ right, top, 1.f }
						};

						auto setPos = [sideMasks, sideSigns](int side, Mesh::Vertex& dst, const XMFLOAT3& pos)
						{
							getElem(dst.position, sideMasks[side][0]) = pos.x * sideSigns[side][0];
							getElem(dst.position, sideMasks[side][1]) = pos.y * sideSigns[side][1];
							getElem(dst.position, sideMasks[side][2]) = pos.z * sideSigns[side][2];
							dst.position = math::normalizeVec3(dst.position);
						};

						setPos(side, vertex[2], quad[0]);
						setPos(side, vertex[1], quad[1]);
						setPos(side, vertex[0], quad[2]);

						vertex[0].normal = math::normalizeVec3(vertex[0].position);
						vertex[1].normal = math::normalizeVec3(vertex[1].position);
						vertex[2].normal = math::normalizeVec3(vertex[2].position);
						
						vertex += 3;

						setPos(side, vertex[2], quad[2]);
						setPos(side, vertex[1], quad[1]);
						setPos(side, vertex[0], quad[3]);

						vertex[0].normal = math::normalizeVec3(vertex[0].position);
						vertex[1].normal = math::normalizeVec3(vertex[1].position);
						vertex[2].normal = math::normalizeVec3(vertex[2].position);
						
						vertex += 3;
					}
				}
			}

			model->updateTriangleOctrees();
			model->init(meshes[0].vertices, meshes);
			GetInstance()->m_Models.emplace(std::make_pair("UNIT_SPHERE_FLAT", model));
			return model;
		}
		return nullptr;
	}

	std::shared_ptr<Model> ModelManager::initUnitCube()
	{
		auto it = GetInstance()->m_Models.find("UNIT_CUBE");

		if (it != GetInstance()->m_Models.end())
		{
			return it->second;
		}
		else
		{
			std::vector<XMFLOAT3> vertices = { 
				{ 0.5f,-0.5f, -0.5f }, 
				{-0.5f,-0.5f, -0.5f }, 
				{-0.5f, 0.5f, -0.5f }, 
				{ 0.5f,-0.5f, -0.5f }, 
				{-0.5f, 0.5f, -0.5f }, 
				{ 0.5f, 0.5f, -0.5f }, 
				{-0.5f,-0.5f,  0.5f }, 
				{-0.5f,-0.5f, -0.5f }, 
				{ 0.5f,-0.5f, -0.5f }, 
				{-0.5f,-0.5f,  0.5f }, 
				{ 0.5f,-0.5f, -0.5f }, 
				{ 0.5f,-0.5f,  0.5f }, 
				{-0.5f, 0.5f,  0.5f }, 
				{-0.5f, 0.5f, -0.5f }, 
				{-0.5f,-0.5f, -0.5f },
				{-0.5f, 0.5f,  0.5f }, 
				{-0.5f,-0.5f, -0.5f },
				{-0.5f,-0.5f,  0.5f },
				{-0.5f,-0.5f,  0.5f }, 
				{ 0.5f,-0.5f,  0.5f }, 
				{ 0.5f, 0.5f,  0.5f }, 
				{-0.5f,-0.5f,  0.5f }, 
				{ 0.5f, 0.5f,  0.5f }, 
				{-0.5f, 0.5f,  0.5f }, 
				{ 0.5f,-0.5f,  0.5f }, 
				{ 0.5f,-0.5f, -0.5f }, 
				{ 0.5f, 0.5f, -0.5f }, 
				{ 0.5f,-0.5f,  0.5f }, 
				{ 0.5f, 0.5f, -0.5f }, 
				{ 0.5f, 0.5f,  0.5f }, 
				{ 0.5f, 0.5f,  0.5f }, 
				{ 0.5f, 0.5f, -0.5f }, 
				{-0.5f, 0.5f, -0.5f }, 
				{ 0.5f, 0.5f,  0.5f }, 
				{-0.5f, 0.5f, -0.5f }, 
				{-0.5f, 0.5f,  0.5f }, 
			};

			std::vector<XMFLOAT2> texCoords = {
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f}
			};

			std::vector<uint16_t> texInds = { 0, 1, 2, 0, 2, 3 };

			std::shared_ptr<Model> model = std::make_shared<Model>();
			model->name = "UNIT_CUBE";
			model->box = Box::empty();
			model->box.min = XMFLOAT3(-0.5f, -0.5f, -0.5f); // for radius to be equal to 1
			model->box.max = XMFLOAT3(0.5f, 0.5f, 0.5f);
			Model::MeshRange range;
			range.indexNum = 0;
			range.vertexNum = 36;
			range.indexOffset = 0;
			range.vertexOffset = 0;
			model->addMeshRange(range);

			std::vector<Mesh> meshes;
			engine::Mesh& mesh = meshes.emplace_back();
			mesh.indexed = false;
			mesh.name = "UNIT_CUBE";
			mesh.box = model->box;
			mesh.m_transform = XMMatrixIdentity();
			mesh.m_transformInv = XMMatrixIdentity();

			mesh.vertices.resize(36);
			Mesh::Vertex* vertex = mesh.vertices.data();

			for (int i = 0; i != 36; i++)
			{
				vertex->position = vertices[i];
				vertex->texCoords = texCoords[texInds[i % 6]];
				vertex++;
			}

			model->updateTriangleOctrees();
			model->init(meshes[0].vertices, meshes);
			GetInstance()->m_Models.emplace(std::make_pair("UNIT_CUBE", model));
			return model;
		}
		return nullptr;
	}


	void ModelManager::clean()
	{
		for (auto& elem : GetInstance()->m_Models)
		{
			elem.second->clean();
		}
	}
}