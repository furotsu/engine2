#pragma once

#include <string>
#include <utility>
#include <iostream>

#include "renderer.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "postProcess.hpp"

namespace engine
{
	class Controller
	{
	public:
		bool m_rmbDown;
		bool m_lmbDown;
		bool userInputReceived;
		bool speedIncreased;
		bool objectPicked;
		bool dissolutionSpawning;
		std::vector<std::pair<uint32_t, uint32_t>> dissolutionQueue; // .first - modelID, .second - spawnTimePoint
		std::vector<std::pair<uint32_t, uint32_t>> disappearanceQueue;
		bool m_mouseMoved;
		bool m_pickedObjectRotating;
		float m_cameraSpeed;
		float m_mouseSensitivity;

		float m_deltaTime;
		float m_timePoint;


		Camera m_camera;
		PostProcess m_postProcess;

		bool m_buttonsState[256];
		std::vector<uint16_t> m_activeButtons;

		POINT m_pressedPos;
		POINT m_currentPos;
		MeshIntersection m_lastIntersection;

		Controller() = default;

		void init(Window& win, Renderer& scene, float timePoint);
		void clean();
		void processInput();

		void update(float deltaTime, Renderer& scene, Window& window, float timePoint);

		void onKeyDown(uint16_t key);
		void onKeyUp(uint16_t key);
		void onResize(const Window& win);

		void spawnDissolutionModel();
		void startRemovingModel(XMVECTOR NDCPos);

		void processFrame(Window& window, Renderer& scene, float timePoint);

		void pickObject(XMVECTOR NDCPos);
		void sprayDecal(XMVECTOR NDCPos);
		void lmbPressed(POINT& pressedPos);

		void changeCameraSpeed(float increase);
		void moveCamera(const XMVECTOR& direction);
		void rotateCamera(float xOffset, float yOffset);
		void rotateCamera(float direction);
		void changeEv(float valuePerSec);
	};
}
