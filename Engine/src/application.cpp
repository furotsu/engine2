#include "application.hpp"
#include "globals.hpp"
#include "textureManager.hpp"
#include "shaderManager.hpp"
#include "modelManager.hpp"
#include "lightSystem.hpp"
#include "meshSystem.hpp"
#include "transformSystem.hpp"
#include "shadowSystem.hpp"
#include "random.hpp"
#include "particleSystem.hpp"
#include "vegetationSystem.hpp"
#include "decalSystem.hpp"

namespace engine
{
	BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
	{
		switch (dwCtrlType)
		{
		case CTRL_CLOSE_EVENT: 
		{
			return TRUE;
		}
		}

		return FALSE;
	}

	Application::~Application()
	{
		clean();
	}

	bool Application::init(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow, int width, int height)
	{
		timer.init();
		math::setSeed();
		initSingletons();

		this->window = Window(width, height, hInstance, hPrevInstance, lpCmdLine, nCmdShow);

		controller.init(window, scene, timer.getTimePoint());
		return true;
	}

	void Application::clean()
	{
		scene.clean();
		controller.clean();
		window.clean();

		deinitSingletons();
	}

	void Application::initConsole()
	{
		if (!AllocConsole()) {
			return;
		}
		BOOL ret = SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

		FILE* dummy;
		auto s = freopen_s(&dummy, "CONOUT$", "w", stdout);
	}

	void Application::initSingletons()
	{
		Globals::init();
		TextureManager::init();
		ShaderManager::init();
		ModelManager::init();
		LightSystem::init();
		MeshSystem::init();
		TransformSystem::init();
		ShadowSystem::init();
		ParticleSystem::init();
		VegetationSystem::init();
		DecalSystem::init();
	}

	void Application::deinitSingletons()
	{
		DecalSystem::deinit();
		VegetationSystem::deinit();
		ParticleSystem::deinit();
		ShadowSystem::deinit();
		TransformSystem::deinit();
		MeshSystem::deinit();
		LightSystem::deinit();
		ModelManager::deinit();
		ShaderManager::deinit();
		TextureManager::deinit();
		Globals::deinit();
	}

	LRESULT Application::processInput(HWND& hWnd, UINT& message, WPARAM& wParam, LPARAM& lParam, Renderer& scene, Window& w)
	{
		switch (message)
		{
		case WM_DESTROY:
		{
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		} break;
		case WM_SIZE:
		{
			RECT rect = { 0, 0, 0, 0 };
			GetWindowRect(hWnd, &rect);

			window.onResize(rect.right - rect.left, rect.bottom - rect.top);
			scene.onResize(window);
			controller.onResize(window);
			controller.userInputReceived = true;
		} break;
		case WM_MOUSEMOVE:
		{
			controller.m_mouseMoved = true;
		} break;
		case WM_LBUTTONDOWN:
		{
			POINT pressedPos;
			GetCursorPos(&pressedPos);
			//determine position relative to init window
			ScreenToClient(FindWindowA(NULL, "Engine"), &pressedPos);

			controller.lmbPressed(pressedPos);
		} break;
		case WM_LBUTTONUP:
		{
			controller.m_lmbDown = false;
		} break;
		case WM_RBUTTONDOWN:
		{
			GetCursorPos(&controller.m_pressedPos);
			ScreenToClient(FindWindowA(NULL, "Engine"), &controller.m_pressedPos);
			controller.m_rmbDown = true;
			controller.m_currentPos = controller.m_pressedPos;
			
			controller.pickObject(window.screenToNDC(controller.m_currentPos.x, controller.m_currentPos.y));
			
			controller.userInputReceived = true;
		} break;
		case WM_RBUTTONUP:
		{
			controller.m_rmbDown = false;
		}break;
		case WM_KEYDOWN:
		{
			if (wParam == 'F')
			{
				POINT pressedPos;
				GetCursorPos(&pressedPos);
				//determine position relative to init window
				ScreenToClient(FindWindowA(NULL, "Engine"), &pressedPos);
				controller.sprayDecal(window.screenToNDC(controller.m_currentPos.x, controller.m_currentPos.y));
			}
			else if (wParam == 'M')
			{
				POINT pressedPos;
				GetCursorPos(&pressedPos);
				//determine position relative to init window
				ScreenToClient(FindWindowA(NULL, "Engine"), &pressedPos);
				controller.startRemovingModel(window.screenToNDC(controller.m_currentPos.x, controller.m_currentPos.y));
			}
			controller.onKeyDown(wParam);
			controller.userInputReceived = true;
		} break;
		case WM_KEYUP:
		{
			controller.onKeyUp(wParam);
		}break;
		case WM_MOUSEWHEEL:
		{
			controller.changeCameraSpeed(GET_WHEEL_DELTA_WPARAM(wParam));
		}break;
		default:
		{
		}break;
		}

		// Handle any messages the switch statement didn't
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	MSG Application::run(ParallelExecutor& executor)
	{
		MSG msg = { 0 };

		timer.recordTime();

		while (TRUE)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);

				DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					return msg;
			}

			m_deltaTime = timer.elapsed();
			if (m_deltaTime > FRAME_DURATION)
			{
				float timePoint = timer.getTimePoint();
				controller.update(m_deltaTime, scene, window, timePoint);
				controller.processFrame(window, scene, timePoint);
				timer.recordTime();

				std::cout << "fps: " << 1.0f / m_deltaTime << std::endl;
			}
		}

		return msg;
	}
}