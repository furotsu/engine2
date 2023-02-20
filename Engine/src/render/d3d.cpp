#include "d3d.hpp"

namespace engine
{
	// global pointers to most used D3D11 objects for convenience:
	ID3D11Device5* s_device = nullptr;
	ID3D11DeviceContext4* s_devcon = nullptr;
	IDXGIFactory5* s_factory = nullptr;

}