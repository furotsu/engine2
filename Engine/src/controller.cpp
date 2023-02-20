#include "controller.hpp"

#include <chrono>
#include <cmath>
#include "textureLoader.hpp"
#include "globals.hpp"
#include "opaque_instances.hpp"
#include "meshSystem.hpp"
#include "lightSystem.hpp"
#include "particleSystem.hpp"
#include "transformSystem.hpp"
#include "vegetationSystem.hpp"
#include "decalSystem.hpp"
#include "random.hpp"

namespace engine
{
	void Controller::init(Window& win, Renderer& scene, float timePoint)
	{
		std::vector<ShaderInfo> shaderResolve = {
			{ShaderType::VERTEX, L"shaders/resolve.hlsl", "VSMain"},
			{ShaderType::PIXEL,  L"shaders/resolve.hlsl",  "PSMain"}
		};

		std::vector<ShaderInfo> shadersSkybox = {
		{ShaderType::VERTEX, L"shaders/skybox.hlsl", "VSMain"},
		{ShaderType::PIXEL,  L"shaders/skybox.hlsl",  "PSMain"}
		};


		scene.init(win, 2u);

		m_postProcess = PostProcess(START_EV100, shaderResolve);

		// Load knight
		Material mat;
		std::vector<Material> materials;
		XMFLOAT3 size = XMFLOAT3(10.0f, 10.0f, 10.0f);
		{
			mat.properties.roughness = 0.9f;
			mat.properties.metalness = 0.01f;
			mat.properties.hasDiffuseTexture = true;
			mat.properties.hasNormalTexture = true;
			mat.properties.hasRoughnessTexture = true;
			mat.properties.hasMetallicTexture = false;
			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Fur_BaseColor.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Fur_Normal.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Fur_Roughness.dds");
			mat.metallicTexture = nullptr;
			materials.push_back(mat);

			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Legs_BaseColor.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Legs_Normal.dds");
			mat.metallicTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Legs_Metallic.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Legs_Roughness.dds");
			mat.properties.hasMetallicTexture = true;
			materials.push_back(mat);

			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Torso_BaseColor.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Torso_Normal.dds");
			mat.metallicTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Torso_Metallic.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Torso_Roughness.dds");
			materials.push_back(mat);

			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Head_BaseColor.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Head_Normal.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Head_Roughness.dds");
			mat.metallicTexture = nullptr;
			mat.properties.hasMetallicTexture = false;
			materials.push_back(mat);

			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Eye_BaseColor.dds");
			mat.normalMapTexture = nullptr;
			mat.roughnessTexture = nullptr;
			mat.properties.hasRoughnessTexture = false;
			mat.properties.hasNormalTexture = false;
			materials.push_back(mat);

			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Helmet_BaseColor.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Helmet_Normal.dds");
			mat.metallicTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Helmet_Metallic.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Helmet_Roughness.dds");
			mat.properties.hasRoughnessTexture = true;
			mat.properties.hasNormalTexture = true;
			mat.properties.hasMetallicTexture = true;
			materials.push_back(mat);

			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Skirt_BaseColor.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Skirt_Normal.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Skirt_Roughness.dds");
			mat.metallicTexture = nullptr;
			mat.properties.hasMetallicTexture = false;
			materials.push_back(mat);

			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Cape_BaseColor.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Cape_Normal.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Cape_Roughness.dds");
			materials.push_back(mat);

			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Glove_BaseColor.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Glove_Normal.dds");
			mat.metallicTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Glove_Metallic.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Glove_Roughness.dds");
			mat.properties.hasMetallicTexture = true;
			materials.push_back(mat);
			
			std::vector<std::vector<OpaqueInstances::Instance>> knightPositions(1);
			knightPositions[0].push_back(TransformSystem::GetInstance()->addTransform(XMMatrixMultiply(XMMatrixScaling(size.x, size.y, size.z), XMMatrixTranslation(8.0f, 10.0f, 15.0f))));
			size = XMFLOAT3(15.0f, 15.0f, 15.0f);
			knightPositions[0].push_back(TransformSystem::GetInstance()->addTransform(XMMatrixMultiply(XMMatrixScaling(size.x, size.y, size.z), XMMatrixTranslation(-8.0f, 7.0f, 15.0f))));

			MeshSystem::GetInstance()->addOpaqueModel(ModelManager::GetInstance()->getModel("assets/models/Knight/", "Knight.fbx"), knightPositions, materials);
			materials.clear();
		}

		// Load cubes
		{
			size = XMFLOAT3(10.0f, 10.0f, 10.0f);
			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Cube/cube.fbm/Cube_brick_texture.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Cube/cube.fbm/Cube_normal_texture.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Cube/cube.fbm/Cube_roughness_texture.dds");
			mat.properties.hasNormalTexture = true;
			mat.properties.hasRoughnessTexture = true;
			mat.properties.hasMetallicTexture = false;
			mat.metallicTexture = nullptr;
			materials.push_back(mat);
			std::vector<std::vector<OpaqueInstances::Instance>> cubePositions(1);
			cubePositions[0].push_back(TransformSystem::GetInstance()->addTransform(XMMatrixMultiply(XMMatrixScaling(size.x, size.y, size.z), XMMatrixTranslation(10.0f, 10.0f, .0f))));
			MeshSystem::GetInstance()->addOpaqueModel(ModelManager::GetInstance()->getModel("assets/models/Cube/", "cube.fbx"), cubePositions, materials);
			materials.clear();

			size = XMFLOAT3(20.0f, 20.0f, 20.0f);

			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Cube/cube.fbm/Stone_COLOR.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Cube/cube.fbm/Stone_NORM.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Cube/cube.fbm/Stone_ROUGH.dds");
			mat.properties.hasNormalTexture = true;
			mat.properties.hasRoughnessTexture = true;
			mat.properties.hasMetallicTexture = false;
			mat.metallicTexture = nullptr;
			materials.push_back(mat);
			cubePositions[0].clear();
			cubePositions[0].push_back(TransformSystem::GetInstance()->addTransform(XMMatrixMultiply(XMMatrixScaling(size.x, size.y, size.z), XMMatrixTranslation(-10.0f, 10.0f, .0f))));
			MeshSystem::GetInstance()->addOpaqueModel(ModelManager::GetInstance()->getModel("assets/models/Cube/", "cube.fbx"), cubePositions, materials);
		}

		scene.setSkybox(Sky("assets/night_street.dds"), shadersSkybox, "assets/night_street_irradiance.dds", "assets/night_street_reflection.dds", "assets/reflectance.dds");

		//Load terrain
		{
			size = XMFLOAT3(1000.0f, 1000.0f, 1000.0f);
			std::vector<std::vector<OpaqueInstances::Instance>> terrainPosition(1);

			terrainPosition[0].push_back({ TransformSystem::GetInstance()->addTransform(XMMatrixMultiply(XMMatrixScaling(size.x, size.y, size.z), XMMatrixTranslation(0.0f, -500.0f, 0.0f))), 30});
			MeshSystem::GetInstance()->setTerrainID(terrainPosition[0][0].transformID);
			mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/terrain/Mud_Albedo.dds");
			mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/terrain/Mud_Normal.dds");
			mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/terrain/Mud_Roughness.dds");
			mat.properties.hasNormalTexture = true;
			mat.properties.hasRoughnessTexture = true;
			mat.properties.hasMetallicTexture = false;
			mat.metallicTexture = nullptr;
			materials.clear();
			materials.push_back(mat);
			MeshSystem::GetInstance()->addOpaqueModel(ModelManager::GetInstance()->getModel("assets/models/Cube/", "cube.fbx"), terrainPosition, materials);

			
		}
		//Load lights
		{
			PointLight pl;
			pl.properties.color = XMFLOAT3(0.95f, 0.7f, 0.55f);
			pl.power = 3000.0f;
			pl.properties.radius = 1.0f;

			std::vector<EmissiveInstances::Instance> emissiveInstances = { TransformSystem::GetInstance()->addTransform(XMMatrixTranslation(2.0f, 25.0f, 0.0f))};
			LightSystem::GetInstance()->addPointLight(pl, emissiveInstances);

			pl.properties.color = XMFLOAT3(0.6f, 0.9f, 0.1f);
			pl.power = 3000.0f;
			emissiveInstances = { TransformSystem::GetInstance()->addTransform(XMMatrixTranslation(-2.0f, 25.0f, 0.0f)) };
			LightSystem::GetInstance()->addPointLight(pl, emissiveInstances);
		}

		//Load particle system
		{
			auto emission = TextureManager::GetInstance()->getTexture("assets/smoke/DDS/emission.dds");
			auto EMVA     = TextureManager::GetInstance()->getTexture("assets/smoke/DDS/Alpha.dds");
			auto RLT =		TextureManager::GetInstance()->getTexture("assets/smoke/DDS/RLT.dds");
			auto BotBF =	TextureManager::GetInstance()->getTexture("assets/smoke/DDS/BotBF.dds");
			auto spark =	TextureManager::GetInstance()->getTexture("assets/spark.dds");
			ParticleSystem::GetInstance()->addSmokeEmitter(XMFLOAT3(10.0f, 1.0f, 0.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 0.05f, 0.0f, emission, EMVA, RLT, BotBF);
			ParticleSystem::GetInstance()->addSmokeEmitter(XMFLOAT3(-10.0f, 1.0f, 3.0f), XMFLOAT4(0.99f, 0.0f, 0.0f, 0.5f), 0.1f, 0.0f, emission, EMVA, RLT, BotBF);
			ParticleSystem::GetInstance()->setParticleTexture(spark);
		}

		//Load grass
		{
			GrassField field;
			std::shared_ptr<Texture> albedo = TextureManager::GetInstance()->getTexture("assets/PBR_grass/DDS/Albedo.dds");
			std::shared_ptr<Texture> normal = TextureManager::GetInstance()->getTexture("assets/PBR_grass/DDS/Normal.dds");
			std::shared_ptr<Texture> opacity = TextureManager::GetInstance()->getTexture("assets/PBR_grass/DDS/Opacity.dds");
			std::shared_ptr<Texture> occlusion = TextureManager::GetInstance()->getTexture("assets/PBR_grass/DDS/AO.dds");
			VegetationSystem::GetInstance()->addGrassField(100.0f, XMFLOAT3(-50.0f, -10.0f, -50.0f), albedo, normal, opacity, occlusion, timePoint);
		}

		//Load decalSystem data
		{
			auto normalTexture = TextureManager::GetInstance()->getTexture("assets/decalNormals.dds");
			DecalSystem::GetInstance()->setDefaultNormalTexture(normalTexture);
		}

		m_camera = Camera(XMVectorSet(0.0f, 25.0f, -30.0f, 1.0f), { 0.0f, 0.0f, 0.0f });
		m_camera.setPerspective(45.0f, win.m_width, win.m_height, CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE);
		m_cameraSpeed = CAMERA_SPEED;
		m_mouseSensitivity = MOUSE_SENSITIVITY;
		m_lastIntersection.reset(SMALL_OFFSET);

		m_rmbDown = false;
		m_lmbDown = false;
		m_pickedObjectRotating = false;
		userInputReceived = true;
		objectPicked = false;
		dissolutionSpawning = false;
	}

	void Controller::clean()
	{
		
	}

	void Controller::update(float deltaTime, Renderer& scene, Window& window, float timePoint)
	{
		m_deltaTime = deltaTime;
		m_timePoint = timePoint;
		processInput();

		if (m_mouseMoved)
		{
			GetCursorPos(&m_currentPos);
			ScreenToClient(FindWindowA(NULL, "Engine"), &m_currentPos);
			if (m_lmbDown)
			{
				rotateCamera((m_currentPos.x - m_pressedPos.x) / (float)window.m_width, (m_currentPos.y - m_pressedPos.y) / (float)window.m_height);
				userInputReceived = true;
			}
		}

		if (userInputReceived)
		{
			if (m_rmbDown)
			{
				if (objectPicked)
				{
					GetCursorPos(&m_currentPos);
					ScreenToClient(FindWindowA(NULL, "Engine"), &m_currentPos);

					XMVECTOR point = window.screenToNDC(m_currentPos.x, m_currentPos.y);

					Ray ray;
					ray.r.origin = XMFLOAT3(XMVectorGetX(m_camera.position()), XMVectorGetY(m_camera.position()), XMVectorGetZ(m_camera.position()));

					XMVECTOR direction = XMVector4Transform(point, m_camera.getProjInv());
					direction /= XMVectorGetW(direction);
					direction = XMVector4Transform(direction, m_camera.getViewInv());
					direction = -XMLoadFloat3(&ray.r.origin) + direction;
					direction = XMVector3Normalize(direction);

					ray.r.direction = XMFLOAT3(XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction));

					if (m_mouseMoved && !m_lmbDown)
					{
						m_lastIntersection.intersectionPlane.setNormal(m_camera.forward());
						Intersection intersection;
						intersection.reset();
						m_lastIntersection.intersectionPlane.hit(ray.r, intersection);
					
						m_lastIntersection.pos = intersection.point;
					
						TransformSystem::GetInstance()->moveTo(m_lastIntersection.transformID, intersection.point - m_lastIntersection.offset);
					
						XMFLOAT3 position = TransformSystem::GetInstance()->getPositionVec3(m_lastIntersection.transformID);
						m_lastIntersection.distToPickedObj = math::lengthVec3(position - ray.r.origin);
						m_pickedObjectRotating = false;
					}
					else
					{
						float clickOffset = 0.0f;
						if (!m_pickedObjectRotating)
						{
							float clickOffset = m_lastIntersection.distToPickedObj - math::lengthVec3(m_lastIntersection.pos - ray.r.origin);
							m_pickedObjectRotating = true;
							m_lastIntersection.distToPickedObj = math::lengthVec3(m_lastIntersection.pos - ray.r.origin);
						}
					
						XMFLOAT3 holdPoint = ray.r.origin + ray.r.direction * (m_lastIntersection.distToPickedObj - clickOffset);
					
						XMFLOAT3 offset = holdPoint - m_lastIntersection.pos;
						
						m_lastIntersection.pos = holdPoint;
						TransformSystem::GetInstance()->moveTo(m_lastIntersection.transformID, m_lastIntersection.pos - m_lastIntersection.offset);
						m_lastIntersection.intersectionPlane.setCenter(m_lastIntersection.pos );
					}

					MeshSystem::GetInstance()->updateOpaqueInstanceBuffer();
					MeshSystem::GetInstance()->updateEmissiveInstanceBuffer();
					DecalSystem::GetInstance()->updateInstanceBuffers();

				}
			}
		}

		if (dissolutionQueue.size())
		{
			uint16_t deleterPos = 0;
			for (int i = 0; i != dissolutionQueue.size(); i++)
			{
				if (m_timePoint - dissolutionQueue[i].second > DISSOLUTION_SPAWNTIME)
				{
					MeshSystem::GetInstance()->addOpaqueModel(MeshSystem::GetInstance()->getDissolutionInstance(dissolutionQueue[i].first));
					MeshSystem::GetInstance()->deleteDissolutionInstance(dissolutionQueue[i].first);
					deleterPos++;
					MeshSystem::GetInstance()->updateOpaqueInstanceBuffer();
				}
			}
			if (deleterPos)
			{
				dissolutionQueue.erase(dissolutionQueue.begin(), dissolutionQueue.begin() + deleterPos);
				dissolutionSpawning = false;
			}
		}

		if (disappearanceQueue.size())
		{
			uint16_t deleterPos = 0;
			for (int i = 0; i != disappearanceQueue.size(); i++)
			{
				if ((m_timePoint - disappearanceQueue[i].second) > DISAPPEARANCE_ERASETIME)
				{
					MeshSystem::GetInstance()->deleteDisappearanceModel(disappearanceQueue[i].first);
					deleterPos++;
				}
			}
			if(deleterPos)
				disappearanceQueue.erase(disappearanceQueue.begin(), disappearanceQueue.begin() + deleterPos);
		}

		LightSystem::GetInstance()->updateIrradiance(m_camera);
		LightSystem::GetInstance()->updateLightsPositions();
		ParticleSystem::GetInstance()->update(m_timePoint);
		m_camera.updateMatrices();
	}

	void Controller::onKeyDown(uint16_t key)
	{
		userInputReceived = true;

		if (!m_buttonsState[key])
		{
			m_activeButtons.push_back(key);
			m_buttonsState[key] = true;
		}
	}

	void Controller::onKeyUp(uint16_t key)
	{
		m_activeButtons.erase(std::remove(m_activeButtons.begin(), m_activeButtons.end(), key), m_activeButtons.end());
		m_buttonsState[key] = false;
	}

	void Controller::onResize(const Window& win)
	{
		m_camera.setPerspective(45.0f, win.m_width, win.m_height, NEAR_PLANE, FAR_PLANE);
	}

	void Controller::spawnDissolutionModel()
	{
		Material mat;
		mat.noiseTexture = TextureManager::GetInstance()->getTexture("assets/noiseTexture.dds");
		std::vector<Material> materials;
		XMFLOAT3 size = XMFLOAT3(10.0f, 10.0f, 10.0f);
		mat.properties.roughness = 0.9f;
		mat.properties.metalness = 0.01f;
		mat.properties.hasDiffuseTexture = true;
		mat.properties.hasNormalTexture = true;
		mat.properties.hasRoughnessTexture = true;
		mat.properties.hasMetallicTexture = false;
		mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Fur_BaseColor.dds");
		mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Fur_Normal.dds");
		mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Fur_Roughness.dds");
		mat.metallicTexture = nullptr;
		materials.push_back(mat);

		mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Legs_BaseColor.dds");
		mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Legs_Normal.dds");
		mat.metallicTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Legs_Metallic.dds");
		mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Legs_Roughness.dds");
		mat.properties.hasMetallicTexture = true;
		materials.push_back(mat);

		mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Torso_BaseColor.dds");
		mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Torso_Normal.dds");
		mat.metallicTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Torso_Metallic.dds");
		mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Torso_Roughness.dds");
		materials.push_back(mat);

		mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Head_BaseColor.dds");
		mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Head_Normal.dds");
		mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Head_Roughness.dds");
		mat.metallicTexture = nullptr;
		mat.properties.hasMetallicTexture = false;
		
		materials.push_back(mat);

		mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Eye_BaseColor.dds");
		mat.normalMapTexture = nullptr;
		mat.roughnessTexture = nullptr;
		mat.properties.hasRoughnessTexture = false;
		mat.properties.hasNormalTexture = false;
		materials.push_back(mat);

		mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Helmet_BaseColor.dds");
		mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Helmet_Normal.dds");
		mat.metallicTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Helmet_Metallic.dds");
		mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Helmet_Roughness.dds");
		mat.properties.hasRoughnessTexture = true;
		mat.properties.hasNormalTexture = true;
		mat.properties.hasMetallicTexture = true;
		materials.push_back(mat);

		mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Skirt_BaseColor.dds");
		mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Skirt_Normal.dds");
		mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Skirt_Roughness.dds");
		mat.metallicTexture = nullptr;
		mat.properties.hasMetallicTexture = false;
		materials.push_back(mat);

		mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Cape_BaseColor.dds");
		mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Cape_Normal.dds");
		mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Cape_Roughness.dds");
		materials.push_back(mat);

		mat.diffuseTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Glove_BaseColor.dds");
		mat.normalMapTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Glove_Normal.dds");
		mat.metallicTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Glove_Metallic.dds");
		mat.roughnessTexture = TextureManager::GetInstance()->getTexture("assets/models/Knight/dds/Glove_Roughness.dds");
		mat.properties.hasMetallicTexture = true;
		materials.push_back(mat);

		std::vector<std::vector<DissolutionInstances::Instance>> knightPositions(1);
		XMFLOAT3 forward;
		XMStoreFloat3(&forward, m_camera.forward());
		XMFLOAT3 position = m_camera.positionVec3() + 15.0f * forward - XMFLOAT3(0.0f, size.y, 0.0f);

		XMMATRIX rotation = m_camera.getView();
		rotation.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		rotation = XMMatrixTranspose(rotation);

		XMMATRIX matrix = XMMatrixMultiply(XMMatrixScaling(size.x, size.y, size.z), rotation);

		knightPositions[0].push_back({ TransformSystem::GetInstance()->addTransform(XMMatrixMultiply(matrix, XMMatrixTranslation(position.x, position.y, position.z))), m_timePoint});

		uint32_t modelID = MeshSystem::GetInstance()->addDissolutionModel(ModelManager::GetInstance()->getModel("assets/models/Knight/", "Knight.fbx"), knightPositions, materials);
		materials.clear();
		dissolutionQueue.push_back({ modelID, m_timePoint });
	}

	void Controller::startRemovingModel(XMVECTOR NDCPos)
	{
		Ray ray;
		ray.r.origin = XMFLOAT3(XMVectorGetX(m_camera.position()), XMVectorGetY(m_camera.position()), XMVectorGetZ(m_camera.position()));

		XMVECTOR direction = XMVector4Transform(NDCPos, m_camera.getProjInv());
		direction /= XMVectorGetW(direction);
		direction = XMVector4Transform(direction, m_camera.getViewInv());
		direction = -XMLoadFloat3(&ray.r.origin) + direction;
		direction = XMVector3Normalize(direction);

		ray.r.direction = XMFLOAT3(XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction));

		MeshIntersection intersection;

		intersection.reset(SMALL_OFFSET);
		if (MeshSystem::GetInstance()->intersects(ray, intersection))
		{
			DisappearanceInstances::Instance::Sphere sphere;
			
			sphere.pos = intersection.intersectionPlane.getCenter();
			XMVECTOR scale;
			XMVECTOR rot;
			XMVECTOR transf;
			XMMatrixDecompose(&scale, &rot, &transf, TransformSystem::GetInstance()->getTransform(intersection.transformID));
			float scale2 = max(XMVectorGetX(scale), max(XMVectorGetY(scale), XMVectorGetZ(scale)));
			sphere.growthSpeed = scale2 / DISAPPEARANCE_ERASETIME * 2.0f;

			uint32_t id = MeshSystem::GetInstance()->addDisappearanceModel(MeshSystem::GetInstance()->getOpaqueInstance(intersection.instanceID), m_timePoint, sphere);

			if (id != -1)
			{
				MeshSystem::GetInstance()->deleteOpaqueInstance(intersection.instanceID);
				MeshSystem::GetInstance()->updateOpaqueInstanceBuffer();

				disappearanceQueue.push_back({ id, m_timePoint });
			}
		}
	}

	void Controller::processFrame(Window& window, Renderer& scene, float timePoint)
	{
		Globals::GetInstance()->bind(window, m_camera, m_postProcess, timePoint, m_deltaTime);

		scene.renderFrame(m_camera, window, m_postProcess, timePoint);
	}

	void Controller::pickObject(XMVECTOR NDCPos)
	{
		Ray ray;
		ray.r.origin = XMFLOAT3(XMVectorGetX(m_camera.position()), XMVectorGetY(m_camera.position()), XMVectorGetZ(m_camera.position()));

		XMVECTOR direction = XMVector4Transform(NDCPos, m_camera.getProjInv());
		direction /= XMVectorGetW(direction);
		direction = XMVector4Transform(direction, m_camera.getViewInv());
		direction = -XMLoadFloat3(&ray.r.origin) + direction;
		direction = XMVector3Normalize(direction);

		ray.r.direction = XMFLOAT3(XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction));

		m_lastIntersection.reset(SMALL_OFFSET);
		objectPicked = MeshSystem::GetInstance()->intersects(ray, m_lastIntersection);
	}

	void Controller::sprayDecal(XMVECTOR NDCPos)
	{
		Ray ray;
		ray.r.origin = XMFLOAT3(XMVectorGetX(m_camera.position()), XMVectorGetY(m_camera.position()), XMVectorGetZ(m_camera.position()));

		XMVECTOR direction = XMVector4Transform(NDCPos, m_camera.getProjInv());
		direction /= XMVectorGetW(direction);
		direction = XMVector4Transform(direction, m_camera.getViewInv());
		direction = -XMLoadFloat3(&ray.r.origin) + direction;
		direction = XMVector3Normalize(direction);

		ray.r.direction = XMFLOAT3(XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction));

		MeshIntersection intersection;

		intersection.reset(SMALL_OFFSET);
		if (MeshSystem::GetInstance()->intersects(ray, intersection))
		{
			float dot = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&intersection.normal), m_camera.forward()));
			
			dot = max(0.01f, 1.0f - dot);
			XMMATRIX scale = XMMatrixScaling(2.0f, 2.0f, dot * 10.0f);
			float angle = math::randRangef(0.0f, 90.0f);
			float sinAngle = sinf(angle);
			float cosAngle = cosf(angle);
			XMMATRIX rot = XMMatrixSet(cosAngle, -sinAngle, 0.0f, 0.0f,
										sinAngle, cosAngle, 0.0f, 0.0f,
										0.0f, 0.0f, 1.0f, 0.0f,
										0.0f, 0.0f, 0.0f, 1.0f);

			XMMATRIX transform = (m_camera.getView());
			transform.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			transform = XMMatrixTranspose(transform);
			
			XMFLOAT3 translation = intersection.intersectionPlane.getCenter();
			
			XMMATRIX f = XMMatrixMultiply(scale, XMMatrixMultiply(rot, transform));
			
			f.r[3] = XMVectorSet(translation.x, translation.y, translation.z, 1.0f);

			DecalSystem::GetInstance()->addDecal(f, intersection.transformID, intersection.instanceID);
		}

	}

	void Controller::lmbPressed(POINT& pressedPos)
	{
		m_lmbDown = true;
		m_pressedPos = pressedPos;
		m_currentPos = pressedPos;
		userInputReceived = true;
	}

	void Controller::processInput()
	{
		//iterate through all possible buttons
		for (uint16_t& button : m_activeButtons)
		{
			switch (button)
			{
			case 'W':
			{
				moveCamera(m_camera.forward()); 
			} break;
			case 'A':
			{
				moveCamera(-m_camera.right());
			} break;
			case 'S':
			{
				moveCamera(-m_camera.forward());
			} break;
			case 'D':
			{
				moveCamera(m_camera.right());
			} break;
			case 'Q':
			{
				moveCamera(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
			}break;
			case 'E':
			{
				moveCamera(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			}break;
			case 'N':
			{
				if (!dissolutionSpawning)
				{
					spawnDissolutionModel();
					dissolutionSpawning = true;
				}
			}break;
			case VK_OEM_PLUS:
			{
				changeEv(3.0f);
			}break;
			case VK_OEM_MINUS:
			{
				changeEv(-3.0f);
			}break;
			case VK_SHIFT:
			{
				speedIncreased = true;
			}break;
			default:
			{
			}break;
			}
		}
	}

	void Controller::changeEv(float valuePerSec)
	{
		m_postProcess.changeEV100(m_deltaTime * valuePerSec);
	}

	void Controller::changeCameraSpeed(float increase)
	{
		float delta = math::lerp(1.0f, 1.1f, static_cast<float>(abs(increase)) / 120.0f);
		if (increase > 0)
			m_cameraSpeed *= (delta);
		else
			m_cameraSpeed /= (delta);
		m_cameraSpeed = math::clamp(m_cameraSpeed, MIN_CAMERA_SPEED, MAX_CAMERA_SPEED);
	}

	void Controller::moveCamera(const XMVECTOR& direction)
	{
		XMVECTOR offset = direction * m_deltaTime * m_cameraSpeed * ((speedIncreased) ? 5.0f : 1.0f);
		speedIncreased = false;
		m_camera.addWorldOffset(offset);
		m_camera.updateMatrices();

		if (objectPicked)
		{
			m_lastIntersection.intersectionPlane.moveCenter(offset);
		}
	}

	void Controller::rotateCamera(float xOffset, float yOffset)
	{
		Angles res;

		res.pitch = yOffset * m_deltaTime * m_mouseSensitivity;
		res.roll = 0.0f;
		res.yaw = xOffset * m_deltaTime * m_mouseSensitivity;

		m_camera.addRelativeAngles(res);

		if (objectPicked)
		{
			m_lastIntersection.intersectionPlane.setNormal(m_camera.forward());
		}
	}

	void Controller::rotateCamera(float direction)
	{
		Angles res;

		res.pitch = 0.0f;
		float d = (direction > 0) ? -1.0f : 1.0f;
		res.roll = d * m_deltaTime * ROLL_ROTATION_SPEED;
		res.yaw = 0.0f;

		m_camera.addRelativeAngles(res);

		if (objectPicked)
		{
			m_lastIntersection.intersectionPlane.setNormal(m_camera.forward());
		}
	}
}