
#include <windows.h>
#include <windowsx.h>

#include "application.hpp"
#include "globals.hpp"
#include "textureManager.hpp"
#include "shaderManager.hpp"
#include "modelManager.hpp"
#include "meshSystem.hpp"
#include "transformSystem.hpp"
#include "lightSystem.hpp"

const uint32_t engine::ParallelExecutor::MAX_THREADS = max(1u, std::thread::hardware_concurrency());
const uint32_t engine::ParallelExecutor::HALF_THREADS = max(1u, std::thread::hardware_concurrency() / 2);

engine::Application app;
engine::ParallelExecutor executor(max(1u, max(engine::ParallelExecutor::MAX_THREADS - 4u, engine::ParallelExecutor::HALF_THREADS)));

#include <DirectXMath.h>

using namespace DirectX;


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	app.initConsole();

	ALWAYS_ASSERT(app.init(hInstance, hPrevInstance, lpCmdLine, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT));

	MSG msg = app.run(executor);

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return app.processInput(hWnd, message, wParam, lParam, app.scene, app.window);
}

template<typename ...Args>
void printToVisualStudioOutput(Args&&... args)
{
	std::stringstream ss;
	(ss << "???" << args) << std::endl; // Fold expression requires C++17
	OutputDebugStringW(ss.str().c_str());
}
