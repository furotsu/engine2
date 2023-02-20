#include "window.hpp"
#include "debug.hpp"
#include "constants.hpp"

#include "d3d.hpp"


namespace engine
{

	void Window::init(_In_ HINSTANCE& hInstance, _In_opt_ HINSTANCE& hPrevInstance, LPSTR& lpCmdLine, int nCmdShow)
	{
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.lpszClassName = L"WindowClass1";

		RegisterClassEx(&wc);

		RECT wr = { 0, 0, m_width, m_height };    // set the size, but not the position
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size
		m_width = wr.right - wr.left;
		m_height = wr.bottom - wr.top;
		hWnd = CreateWindowEx(NULL,
			L"WindowClass1",    // name of the window class
			L"Engine",   // title of the window
			WS_OVERLAPPEDWINDOW,    // window style
			300,    // x-position of the window
			300,    // y-position of the window
			m_width,    // width of the window
			m_height,    // height of the window
			NULL,    // we have no parent window, NULL
			NULL,    // we aren't using menus, NULL
			hInstance,    // application handle
			NULL);    // used with multiple windows, NULL

		ShowWindow(hWnd, nCmdShow);
		
		hdc = GetDC(hWnd);

		initSwapchain();
		initBackBuffer();
		initViewPort();
		onResize(m_width, m_height); // to adjust height properly

	}

	void Window::clean()
	{
		m_swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode
		m_swapchain.release();
		m_renderTargetView.release();
		m_backbuffer.release();
	}

	void Window::onResize(uint16_t width, uint16_t height)
	{
		if (hWnd)
		{
			m_width = width;
			m_height = height;
			
			m_renderTargetView.release();
			m_swapchain->ResizeBuffers(NULL, m_width, m_height, DXGI_FORMAT_UNKNOWN, NULL);
			
			initBackBuffer();
			initViewPort();
		}
	}

	XMVECTOR Window::screenToNDC( uint16_t x,  uint16_t y) const
	{
		float xNDC = (2.0f * x) / m_width - 1.0f;
		float yNDC = 1.0f - (2.0f * y) / m_height;
		return XMVectorSet(xNDC, yNDC, 1.0f, 1.0f);
	}

	void Window::present()
	{
		m_swapchain->Present(0, 0);
	}

	void Window::bindTarget(ID3D11DepthStencilView* pDSV, float r, float g, float b, float a)
	{
		FLOAT backgroundColor[4] = { r, g, b, a };

		ASSERT(m_renderTargetView != NULL && "render target is NULL");
		
		s_devcon->OMSetRenderTargets(1, m_renderTargetView.access(), pDSV);
		s_devcon->ClearRenderTargetView(m_renderTargetView.ptr(), backgroundColor);
	}

	void Window::initSwapchain()
	{
		DXGI_SWAP_CHAIN_DESC1 desc;

		// clear out the struct for use
		memset(&desc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));

		// fill the swap chain description struct
		desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.BufferCount = 2;
		desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Flags = NULL;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1; // how many samples
		desc.SampleDesc.Quality = 0;                            
		desc.Scaling = DXGI_SCALING_NONE;
		desc.Stereo = false;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		HRESULT res = s_factory->CreateSwapChainForHwnd(s_device, hWnd, &desc, NULL, NULL, m_swapchain.reset());

		ALWAYS_ASSERT(res >= 0 && "CreateSwapChainForHwnd");
	}

	void Window::initBackBuffer() // may be called after resizing
	{
		HRESULT result = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)m_backbuffer.reset());
		ALWAYS_ASSERT(result >= 0);


		m_backbufferDesc.SampleDesc.Count = MULTISAMPLES_COUNT;

		ID3D11Texture2D* pTextureInterface = nullptr;
		m_backbuffer->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		pTextureInterface->GetDesc(&m_backbufferDesc);

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = (MULTISAMPLES_COUNT == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DMS;
		rtvDesc.Texture2D.MipSlice = 0;

		// set the render target as the back buffer
		result = s_device->CreateRenderTargetView(m_backbuffer, &rtvDesc, m_renderTargetView.reset());
		ALWAYS_ASSERT(result >= 0);
		m_backbuffer.release();
		pTextureInterface->Release();
		pTextureInterface = nullptr;
	}

	void Window::initViewPort()
	{
		RECT rect = { 0, 0, 0, 0 };
		GetWindowRect(hWnd, &rect);

		ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = rect.right - rect.left;
		m_viewport.Height = rect.bottom - rect.top;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		s_devcon->RSSetViewports(1, &m_viewport);
	}

	void Window::setViewPort()
	{
		s_devcon->RSSetViewports(1, &m_viewport);
	}

	void Window::setTmpViewPort(uint32_t width, uint32_t height)
	{
		D3D11_VIEWPORT m_viewport;
		ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = width;
		m_viewport.Height = height;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		s_devcon->RSSetViewports(1, &m_viewport);
	}


}