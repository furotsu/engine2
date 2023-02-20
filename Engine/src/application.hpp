#pragma once

#include <iostream>

#include "controller.hpp"
#include "window.hpp"
#include "timer.hpp"
#include "parallelExecutor.hpp"

namespace engine
{
	class Application
	{
	public:
		Window window;
		Controller controller;
		Renderer scene;
		Timer timer;

		float m_deltaTime;

		Application() = default;
		~Application();

		bool init(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow, int width, int height);
		void clean();
		void initConsole();
		void initSingletons();
		void deinitSingletons();

		LRESULT CALLBACK processInput(HWND& hWnd, UINT& message, WPARAM& wParam, LPARAM& lParam, Renderer& scene, Window& w);

		MSG run(ParallelExecutor& executor);

	};

}