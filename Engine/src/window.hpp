#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include <DirectXMath.h>
#include "DxRes.hpp"
#include "debug.hpp"

using namespace DirectX;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
namespace engine
{
	class Window
	{
		D3D11_VIEWPORT m_viewport;
		D3D11_TEXTURE2D_DESC m_backbufferDesc;

	public:
		DxResPtr<ID3D11RenderTargetView> m_renderTargetView;
		DxResPtr<IDXGISwapChain1> m_swapchain;             // the pointer to the swap chain interface
		DxResPtr<ID3D11Texture2D> m_backbuffer;    // the pointer to our back buffer

		uint16_t m_width;
		uint16_t m_height;
		HWND hWnd = nullptr;
		WNDCLASSEX wc;
		HDC hdc;

		Window() = default;
		Window(int width, int height, _In_ HINSTANCE& hInstance, _In_opt_ HINSTANCE& hPrevInstance, LPSTR& lpCmdLine, int nCmdShow)
		{
			m_width = width;
			m_height = height;
			init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
		}

		void init(_In_ HINSTANCE& hInstance, _In_opt_ HINSTANCE& hPrevInstance, LPSTR& lpCmdLine, int nCmdShow);
		void clean();
		void onResize(uint16_t width, uint16_t height);
		XMVECTOR screenToNDC(uint16_t x, uint16_t y) const;

		//DirectX

		void initSwapchain();
		void initBackBuffer();
		void initViewPort();

		void setViewPort();
		void setTmpViewPort(uint32_t width, uint32_t height);


		void present();
		void bindTarget(ID3D11DepthStencilView* pDSV = nullptr, float r = 0.0f, float g = 0.2f, float b = 0.4f, float a = 1.0f);


		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		friend class Scene;
		friend class Renderer;

	private:
	};
}