#include <iostream>

#include "model.hpp"

namespace engine
{
	void IndexBuffer::init(std::vector<Mesh::Triangle>& indices, D3D11_USAGE usage)
	{
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = usage;
		indexBufferDesc.ByteWidth = sizeof(uint32_t) * indices.size() * 3; // every triangle has 3 indices
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = (usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : NULL;
		indexBufferDesc.MiscFlags = NULL;

		D3D11_SUBRESOURCE_DATA indexData;
		ZeroMemory(&indexData, sizeof(indexData));
		indexData.pSysMem = &indices[0];

		s_device->CreateBuffer(&indexBufferDesc, &indexData, m_pIBuffer.reset());
	}
	void IndexBuffer::bind()
	{
		s_devcon->IASetIndexBuffer(m_pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	D3D11_MAPPED_SUBRESOURCE& IndexBuffer::map()
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		s_devcon->Map(m_pIBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		return ms;
	}

	void IndexBuffer::unmap()
	{
		s_devcon->Unmap(m_pIBuffer, NULL);
	}

	void IndexBuffer::clean()
	{
		m_pIBuffer.release();
	}

	void Model::init(std::vector<Mesh::Vertex>& vertices, std::vector<Mesh::Triangle> indices, std::vector<Mesh>& meshes)
	{
		indexed = true;
		m_vertices.init(vertices, D3D11_USAGE_IMMUTABLE);
		m_indices.init(indices, D3D11_USAGE_IMMUTABLE);
		initMeshes(meshes);
	}

	void Model::init(std::vector<Mesh::Vertex>& vertices, std::vector<Mesh>& meshes)
	{
		indexed = false;
		m_vertices.init(vertices, D3D11_USAGE_IMMUTABLE);
		initMeshes(meshes);
	}

	void Model::updateTriangleOctrees()
	{
		for (auto& mesh : meshes)
			mesh.octree.initialize(mesh);
	}

	void Model::clean()
	{
		for (auto& mesh : meshes)
			mesh.clean();

		if (this->indexed)
			m_indices.clean();
		m_vertices.clean();
	}
}