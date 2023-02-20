#include <array>

#include "sky.hpp"


engine::Sky::Sky(std::string filepath)
{
	m_texture = TextureManager::getTexture(filepath, TextureType::TEXTURE_CUBEMAP);
}

void engine::Sky::init(std::vector<ShaderInfo>& shaders, std::string irradianceFile, std::string reflectionFile, std::string reflectanceFile)
{
	m_skyboxProgram = ShaderManager::GetInstance()->getShader(shaders);

	if (m_skyboxProgram->isUniformsEmpty())
	{
		m_skyboxProgram->createUniform(sizeof(XMVECTOR) * 4);
	}

	irradianceMap  = TextureManager::getTexture(irradianceFile, TextureType::TEXTURE_CUBEMAP);
	reflectionMap  = TextureManager::getTexture(reflectionFile, TextureType::TEXTURE_CUBEMAP);
	reflectanceMap = TextureManager::getTexture(reflectanceFile, TextureType::TEXTURE_DEFAULT);
}

void engine::Sky::bindIBLTextures(uint16_t startSlot)
{
	irradianceMap->bind(startSlot);
	reflectionMap->bind(startSlot + 1);
	reflectanceMap->bind(startSlot + 2);
}

void engine::Sky::clean()
{
	m_texture = nullptr;
	m_skyboxProgram = nullptr;
}

void engine::Sky::render(const Camera& camera)
{
	bind();

	XMVECTOR TL = XMVectorSet(-1.0f, 3.0f, 1.0f, 1.0f);
	XMVECTOR BR = XMVectorSet(3.0f, -1.0f, 1.0f, 1.0f);
	XMVECTOR BL = XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f);

	TL = XMVector4Transform(TL, camera.getViewProjInv());
	BR = XMVector4Transform(BR, camera.getViewProjInv());
	BL = XMVector4Transform(BL, camera.getViewProjInv());

	TL /= XMVectorGetW(TL);
	BR /= XMVectorGetW(BR);
	BL /= XMVectorGetW(BL);

	XMVECTOR res[4]{ TL, BR, BL, camera.position() };

	struct vertexCbuffer
	{
		std::array<XMVECTOR, 4> triangle;
	};

	vertexCbuffer* vbuff;

	vbuff = (vertexCbuffer*)m_skyboxProgram->mapUniformVertex(0).pData;
	vbuff->triangle = {TL, BR, BL, camera.position()};
	m_skyboxProgram->unmapUniformVertex(0);
	m_skyboxProgram->bindBufferVertex(0, 0);

	s_devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	s_devcon->Draw(3, 0);
}

void engine::Sky::bind()
{
	m_skyboxProgram->bind();
	m_texture->bind();
}

