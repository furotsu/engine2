#include "texture.hpp"
#include "textureLoader.hpp"
#include "debug.hpp"

void engine::Texture::init(std::string& filepath, TextureType type)
{
	ID3D11Resource* res;

	std::wstring widestr = std::wstring(filepath.begin(), filepath.end());
	const wchar_t* widecstr = widestr.c_str();

	HRESULT hr = DirectX::CreateDDSTextureFromFile(s_device, s_devcon, widecstr, &res, m_shaderResourceView.reset());
	ASSERT(hr >= 0 && L"unable to create texture from file");


	res->Release();
	res = nullptr;
}

void engine::Texture::clean()
{
	m_shaderResourceView.release();
}

void engine::Texture::bind(uint32_t slot)
{
	s_devcon->PSSetShaderResources(slot, 1, m_shaderResourceView.access());
}

void engine::Texture::unbind(uint32_t slot)
{
	s_devcon->PSSetShaderResources(slot, 1, NULL);
	s_devcon->PSSetSamplers(0, 1, NULL);
}
