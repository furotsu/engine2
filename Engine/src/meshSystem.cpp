#include "modelManager.hpp"
#include "shaderManager.hpp"
#include "meshSystem.hpp"
#include "transformSystem.hpp"
#include "lightSystem.hpp"
#include "particleSystem.hpp"
#include "random.hpp"

namespace engine
{
	MeshSystem* MeshSystem::s_manager = nullptr;

	MeshSystem::MeshSystem()
	{

		// shaders for main triangle pipeline
		std::vector<ShaderInfo> shadersOpaque = {
			{ShaderType::VERTEX, L"shaders/opaque.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/opaque.hlsl",  "PSMain"}
		};

		std::vector<ShaderInfo> shadersEmissive = {
			{ShaderType::VERTEX, L"shaders/emissive.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/emissive.hlsl",  "PSMain"}
		};

		std::vector<ShaderInfo> shadersDissolution= {
			{ShaderType::VERTEX, L"shaders/dissolution.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/dissolution.hlsl",  "PSMain"}
		};

		std::vector<ShaderInfo> shadersDisappearance = {
			{ShaderType::VERTEX, L"shaders/disappearance.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/disappearance.hlsl",  "PSMain"}
		};

		std::vector<ShaderInfo> shadersShadowMap = {
			{ShaderType::VERTEX, L"shaders/shadowMap.hlsl", "VSMain"},
			{ShaderType::GEOMETRY,  L"shaders/shadowMap.hlsl",  "GSMain"}
		};

		std::vector<ShaderInfo> shadersShadowMapDissolution = {
			{ShaderType::VERTEX, L"shaders/shadowMapDissolution.hlsl", "VSMain"},
			{ShaderType::GEOMETRY,  L"shaders/shadowMapDissolution.hlsl",  "GSMain"},
			{ShaderType::PIXEL,  L"shaders/shadowMapDissolution.hlsl",  "PSMain"}
		};

		std::vector<ShaderInfo> shadersShadowMapDisappearance = {
			{ShaderType::VERTEX, L"shaders/shadowMapDisappearance.hlsl", "VSMain"},
			{ShaderType::GEOMETRY,  L"shaders/shadowMapDisappearance.hlsl",  "GSMain"},
			{ShaderType::PIXEL,  L"shaders/shadowMapDisappearance.hlsl",  "PSMain"}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedOpaque =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0,   DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"TANGENT", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ROWX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWY",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWZ",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWW",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TSCALE", 0, DXGI_FORMAT_R32_FLOAT,          1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTANCEID", 0, DXGI_FORMAT_R16_UINT,          1, 68, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedDissolution =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0,   DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"TANGENT", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ROWX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWY",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWZ",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWW",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TSCALE", 0, DXGI_FORMAT_R32_FLOAT,          1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TIMEPOINT", 0, DXGI_FORMAT_R32_FLOAT,       1, 68, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTANCEID", 0, DXGI_FORMAT_R16_UINT,          1, 72, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedDisappearance =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0,   DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"TANGENT", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ROWX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWY",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWZ",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWW",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TSCALE", 0, DXGI_FORMAT_R32_FLOAT,          1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TIMEPOINT", 0, DXGI_FORMAT_R32_FLOAT,       1, 68, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"SPHEREPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT,       1, 72, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"SPHEREGROWTHSPEED", 0, DXGI_FORMAT_R32_FLOAT,       1, 84, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTANCEID", 0, DXGI_FORMAT_R16_UINT,          1, 88, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedEmissive =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0,   DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ROWX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedShadowMap =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ROWX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedShadowMapDissolution =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ROWX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			//"TSCALE"
			{"TIMEPOINT", 0, DXGI_FORMAT_R32_FLOAT, 1, 68, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> iedShadowMapDisappearance =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ROWX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ROWW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			//"TSCALE"
			{"TIMEPOINT", 0, DXGI_FORMAT_R32_FLOAT, 1, 68, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"SPHEREPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT,       1, 72, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"SPHEREGROWTHSPEED", 0, DXGI_FORMAT_R32_FLOAT,       1, 84, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		m_opaqueInstances.init(ShaderManager::GetInstance()->getShader(shadersOpaque, iedOpaque), ShaderManager::GetInstance()->getShader(shadersShadowMap, iedShadowMap));
		m_emissiveInstances.init(ShaderManager::GetInstance()->getShader(shadersEmissive, iedEmissive));
		m_dissolutionInstances.init(ShaderManager::GetInstance()->getShader(shadersDissolution, iedDissolution), ShaderManager::GetInstance()->getShader(shadersShadowMapDissolution, iedShadowMapDissolution));
		m_disappearanceInstances.init(ShaderManager::GetInstance()->getShader(shadersDisappearance, iedDisappearance), ShaderManager::GetInstance()->getShader(shadersShadowMapDisappearance, iedShadowMapDisappearance));
		m_disappearanceInstances.setNoiseTexture(TextureManager::GetInstance()->getTexture("assets/noiseTexture2.dds"));
}

	void MeshSystem::init()
	{
		if (s_manager == nullptr)
		{
			s_manager = new MeshSystem();
		}
		else
		{
			ASSERT(false && "Initializing \" ModelManager \" singleton more than once ");
		}
	}

	void MeshSystem::deinit()
	{
		if (s_manager == nullptr)
		{
			ASSERT(false && "Trying to delete \" ModelManager \" singleton more than once ");
		}
		else
		{
			GetInstance()->clean();
			delete s_manager;
			s_manager = nullptr;
		}
	}

	MeshSystem* MeshSystem::GetInstance()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		if (s_manager == nullptr)
		{
			ERROR("Trying to call \" TextureManager \" singleton  instance before initializing it");
		}
#endif
		return s_manager;
	}

	void MeshSystem::addOpaqueModel(std::shared_ptr<Model> model, std::vector<std::vector<OpaqueInstances::Instance>> instances, std::vector<Material>& materials)
	{
		uint32_t modelPos = m_opaqueInstances.addModel(model);

		m_opaqueInstances.addMaterials(materials, modelPos);

		for (uint32_t i = 0; i != instances.size(); ++i)
		{
			for (uint32_t j = 0; j != instances[i].size(); j++)
			{
				instances[i][j].setID(generateID());
			}

			m_opaqueInstances.addInstances(instances[i], modelPos, i);
		}
	}

	void MeshSystem::addOpaqueModel(DissolutionInstances::PerModel model)
	{
		m_opaqueInstances.addModel(model, generateID());
	}

	uint32_t MeshSystem::addDissolutionModel(std::shared_ptr<Model> model, std::vector<std::vector<DissolutionInstances::Instance>> instances, std::vector<Material>& materials)
	{
		uint32_t modelPos = m_dissolutionInstances.addModel(model);
		uint32_t ID = generateID();
		m_dissolutionInstances.addMaterials(materials, modelPos);

		for (uint32_t i = 0; i != instances.size(); ++i)
		{
			for (uint32_t j = 0; j != instances[i].size(); j++)
			{
				instances[i][j].setID(ID);
			}
			m_dissolutionInstances.addInstances(instances[i], modelPos, i);
		}
		m_dissolutionInstances.updateInstanceBuffers();
		return ID;
	}

	DissolutionInstances::PerModel MeshSystem::getDissolutionInstance(uint32_t instanceID)
	{
		bool instanceFound = false;

		std::vector<DissolutionInstances::Instance> instances;
		std::vector<DissolutionInstances::PerMaterial> materials;
		std::vector<DissolutionInstances::PerMesh> meshes;

		for (auto& model : m_dissolutionInstances.perModel)
		{
			DissolutionInstances::PerMesh copyMesh;
			for (auto& mesh : model.perMesh)
			{
				for (auto& material : mesh.perMaterial)
				{
					for (auto& instance : material.instances)
					{
						if (instance.instanceID == instanceID)
						{
							instanceFound = true;
							instances.push_back(instance);
						}
					}
					if (instanceFound)
					{
						DissolutionInstances::PerMaterial mat;
						mat.instances = instances;
						instances.clear();
						mat.material = material.material;
						materials.push_back(mat);
					}
				}
				if (instanceFound)
				{
					DissolutionInstances::PerMesh mesh2;
					mesh2.perMaterial = materials;
					materials.clear();
					mesh2.transform = mesh.transform;
					meshes.push_back(mesh2);
				}
			}
			if (instanceFound)
			{
				DissolutionInstances::PerModel perModel;
				perModel.model = model.model;
				perModel.perMesh = meshes;
				return perModel;
			}
		}
		DissolutionInstances::PerModel perModel;
		return perModel; //empty
	}

	void MeshSystem::deleteDissolutionInstance(uint32_t instanceID)
	{
		bool instanceFound = false;
		for (auto& model : m_dissolutionInstances.perModel)
		{
			for (auto& mesh : model.perMesh)
			{
				for (auto& material : mesh.perMaterial)
				{
					for (int i = 0; i != material.instances.size(); i++)
					{
						if (material.instances[i].instanceID == instanceID)
						{
							instanceFound = true;
							material.instances[i] = material.instances.back();
						}
					}
					if (instanceFound)
					{
						material.instances.pop_back();
						instanceFound = false;
					}
				}
			}
		}
	}

	OpaqueInstances::PerModel MeshSystem::getOpaqueInstance(uint32_t instanceID)
	{
		bool instanceFound = false;

		std::vector<OpaqueInstances::Instance> instances;
		std::vector<OpaqueInstances::PerMaterial> materials;
		std::vector<OpaqueInstances::PerMesh> meshes;

		for (auto& model : m_opaqueInstances.perModel)
		{
			OpaqueInstances::PerMesh copyMesh;
			for (auto& mesh : model.perMesh)
			{
				for (auto& material : mesh.perMaterial)
				{
					for (auto& instance : material.instances)
					{
						if (instance.instanceID == instanceID)
						{
							instanceFound = true;
							instances.push_back(instance);
						}
					}
					if (instanceFound)
					{
						OpaqueInstances::PerMaterial mat;
						mat.instances = instances;
						instances.clear();
						mat.material = material.material;
						materials.push_back(mat);
					}
				}
				if (instanceFound)
				{
					OpaqueInstances::PerMesh mesh2;
					mesh2.perMaterial = materials;
					materials.clear();
					mesh2.transform = mesh.transform;
					meshes.push_back(mesh2);
				}
			}
			if (instanceFound)
			{
				OpaqueInstances::PerModel perModel;
				perModel.model = model.model;
				perModel.perMesh = meshes;
				meshes.clear();
				return perModel;
			}
		}
		OpaqueInstances::PerModel perModel;
		return perModel; //empty
	}

	void MeshSystem::deleteOpaqueInstance(uint32_t instanceID)
	{
		bool instanceFound = false;
		for (auto& model : m_opaqueInstances.perModel)
		{
			for (auto& mesh : model.perMesh)
			{
				for (auto& material : mesh.perMaterial)
				{
					for (int i = 0; i != material.instances.size(); i++)
					{
						if (material.instances[i].instanceID == instanceID)
						{
							instanceFound = true;
							material.instances[i] = material.instances.back();
						}
					}
					if (instanceFound)
					{
						material.instances.pop_back();
						instanceFound = false;
					}
				}
			}
		}
	}

	uint32_t MeshSystem::addDisappearanceModel(OpaqueInstances::PerModel model, uint32_t timePoint, DisappearanceInstances::Instance::Sphere sphere)
	{
		if (!model.empty())
		{
			uint32_t id = generateID();
		m_disappearanceInstances.addModel(model, id, timePoint, sphere);
		m_disappearanceInstances.updateInstanceBuffers();
		return id;
		}
		return -1;
	}

	void MeshSystem::deleteDisappearanceModel(uint32_t disappearanceModelID)
	{
		bool instanceFound = false;
		for (auto& model : m_disappearanceInstances.perModel)
		{
			for (auto& mesh : model.perMesh)
			{
				for (auto& material : mesh.perMaterial)
				{
					for (int i = 0; i != material.instances.size(); i++)
					{
						if (material.instances[i].instanceID == disappearanceModelID)
						{
							instanceFound = true;
							material.instances[i] = material.instances.back();
						}
					}
					if (instanceFound)
					{
						material.instances.pop_back();
						instanceFound = false;
					}
				}
			}
		}
		m_disappearanceInstances.updateInstanceBuffers();
	}

	void MeshSystem::addEmissiveModel(std::vector<EmissiveInstances::Instance> instances)
	{
		uint32_t modelPos = m_emissiveInstances.addModel(XMFLOAT3(1.0f, 1.0f, 1.0f));

		std::vector<Material> mat = { Material() };
		m_emissiveInstances.addMaterials(mat, modelPos);

		m_emissiveInstances.addInstances(instances, modelPos, 0);

		m_emissiveInstances.perModel[modelPos].model->updateTriangleOctrees();
	}

	void MeshSystem::updateOpaqueInstanceBuffer()
	{
		m_opaqueInstances.updateInstanceBuffers();
	}

	void MeshSystem::updateEmissiveInstanceBuffer()
	{
		m_emissiveInstances.updateInstanceBuffers();
	}
		
	void MeshSystem::render(Sky& sky)
	{
		m_opaqueInstances.render(sky);
	}

	void MeshSystem::renderEmissive()
	{
		m_emissiveInstances.render();
	}

	void MeshSystem::renderDissolution(Sky& sky)
	{
		m_dissolutionInstances.render(sky);
	}

	void MeshSystem::renderDisappearance(Sky& sky)
	{
		m_disappearanceInstances.render(sky);
	}

	void MeshSystem::spawnDisappearanceParticles(std::shared_ptr<ShaderProgram> shader)
	{
		m_disappearanceInstances.spawnParticles(shader);
	}

	void MeshSystem::renderDepthOnly(const Camera& camera)
	{
		m_dissolutionInstances.renderDepthOnly();
		m_disappearanceInstances.renderDepthOnly();
		m_opaqueInstances.renderDepthOnly();
	}

	bool MeshSystem::intersects(Ray& r, MeshIntersection& intersection)
	{
		bool res = false;
		Ray modelRay = r;
		intersection.t = std::numeric_limits<float>::infinity();

		for (auto& model : m_opaqueInstances.perModel)
		{
			intersection.t = std::numeric_limits<float>::infinity();
			for (int j = 0; j != model.perMesh.size(); j++)
			{
				for (auto& material : model.perMesh[j].perMaterial)
				{
					for (auto& instance : material.instances)
					{
						if (instance.transformID != m_terrainID)
						{
							const Mesh& mesh = model.model->meshes[j];
							
							//combine instance and model transform matrices to then convert ray to model space
							modelRay.r.origin = math::transformVec3(r.r.origin, TransformSystem::GetInstance()->getTransformInverse(instance.transformID));
							modelRay.r.origin = math::transformVec3(modelRay.r.origin, mesh.m_transformInv);
							
							
							//XMMATRIX matrix = XMMatrixInverse(nullptr, XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 45.0f));
							XMVECTOR scale;
							XMVECTOR quaternion;
							XMVECTOR translation;
							XMMatrixDecompose(&scale, &quaternion, &translation, TransformSystem::GetInstance()->getTransformInverse(instance.transformID));
							
							modelRay.r.direction = (math::transformVec3(r.r.direction, XMMatrixRotationQuaternion(quaternion)));
							modelRay.r.direction = math::normalizeVec3(math::transformVec3(modelRay.r.direction, mesh.m_transformInv));

							if (mesh.octree.intersect(modelRay, intersection))
							{
								XMFLOAT3 pos = math::transformVec3(intersection.pos, (mesh.m_transform));
								pos = math::transformVec3(pos, TransformSystem::GetInstance()->getTransform(instance.transformID));
								float distToPickedObj = math::lengthVec3(r.r.origin - pos);
								
								if (distToPickedObj < intersection.distToPickedObj)
								{
									res = true;
									intersection.pos = pos;
									intersection.distToPickedObj = distToPickedObj;	

									intersection.transformID = instance.transformID;
									intersection.instanceID = instance.instanceID;

									//intersection.t = intersection.distToPickedObj;

									intersection.offset = intersection.pos - TransformSystem::GetInstance()->getPositionVec3(instance.transformID);
									intersection.intersectionPlane = math::Plane(r.r.origin - intersection.pos, intersection.pos);

									XMStoreFloat3(&intersection.normal, XMVector3Transform(XMLoadFloat3(&intersection.normal), mesh.m_transform));
									XMStoreFloat3(&intersection.normal, XMVector3Normalize(XMLoadFloat3(&intersection.normal)));
								}
							}
						}
					}
				}
			}

		}
	
	intersection.t = std::numeric_limits<float>::infinity();
	modelRay = r;
	 for (auto& model : m_emissiveInstances.perModel)
	 {
		 for (auto& mesh : model.perMesh)
		 {
			 for (auto& material : mesh.perMaterial)
			 {
				 for (auto& instance : material.instances)
				 {
					 for (auto& mesh : model.model->meshes)
					 {
						 //combine instance and model transform matrices to then convert ray to model space
						 modelRay.r.origin = math::transformVec3(r.r.origin, TransformSystem::GetInstance()->getTransformInverse(instance.transformID));
						 modelRay.r.origin = math::transformVec3(modelRay.r.origin, mesh.m_transformInv);
						
						 //no rotation because it's a sphere
						 modelRay.r.direction = math::normalizeVec3(math::transformVec3(r.r.direction, mesh.m_transformInv));
						 
						 if (mesh.octree.intersectSphere(modelRay, intersection))
						 {
							 XMFLOAT3 pos = math::transformVec3(intersection.pos, (mesh.m_transform));
							 pos = math::transformVec3(pos, TransformSystem::GetInstance()->getTransform(instance.transformID));
							 float distToPickedObj = math::lengthVec3(r.r.origin - pos);

							 if (distToPickedObj < intersection.distToPickedObj)
							 {
								 res = true;
								 intersection.pos = pos;
								 intersection.distToPickedObj = distToPickedObj;
								 intersection.transformID = instance.transformID;
								// intersection.t = intersection.distToPickedObj;
								 intersection.offset = intersection.pos - TransformSystem::GetInstance()->getPositionVec3(instance.transformID);
								 intersection.intersectionPlane = math::Plane(r.r.origin - intersection.pos, intersection.pos);
	
							 }
						 }
					 }
				 }
			 }
		 }
	}
		return res;
	}

	void MeshSystem::clean()
	{
		m_opaqueInstances.clean();
		m_emissiveInstances.clean();
	}
	uint16_t MeshSystem::generateID()
	{
		uint16_t res = m_lastID;
		m_lastID += 1;
		return res;
	}
}
