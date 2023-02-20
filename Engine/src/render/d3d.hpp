#pragma once

#include "win.hpp"

#include "DxRes.hpp"
#include <d3d11_4.h>
#include <d3dcompiler.h>

namespace engine
{
	// global pointers to most used D3D11 objects for convenience:
	extern ID3D11Device5* s_device;
	extern ID3D11DeviceContext4* s_devcon;
	extern IDXGIFactory5* s_factory;
}