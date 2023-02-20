#pragma once

#include "DxRes.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include "texture.hpp"
#include "textureManager.hpp"
#include "shader.hpp"

namespace engine
{
	template<typename T>
	class VertexBuffer
	{
		uint32_t m_size;
		DxResPtr<ID3D11Buffer> m_pVBuffer;
	public:
		VertexBuffer() = default;

		void init(std::vector<T>& vertices, D3D11_USAGE usage)
		{
			m_size = sizeof(T) * vertices.size();
			// create the vertex buffer
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = usage; 
			bd.ByteWidth = m_size; // size is the VERTEX struct * 3
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       
			bd.CPUAccessFlags = (usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : NULL;    

			D3D11_SUBRESOURCE_DATA vertexData;
			ZeroMemory(&vertexData, sizeof(vertexData));
			vertexData.pSysMem = &vertices[0];

			s_device->CreateBuffer(&bd, &vertexData, m_pVBuffer.reset()); // create the buffer

			ASSERT(m_pVBuffer != 0 && "something wrong with vertex buffer");
		};

		void init(uint32_t size, D3D11_USAGE usage)
		{
			m_size = size;
			// create the vertex buffer
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = usage;
			bd.ByteWidth = size * 3; // size is the VERTEX struct * 3
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = (usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : NULL;

			s_device->CreateBuffer(&bd, NULL, m_pVBuffer.reset()); // create the buffer

			ASSERT(m_pVBuffer != 0 && "something wrong with vertex buffer");
		};

		D3D11_MAPPED_SUBRESOURCE map() 
		{
			D3D11_MAPPED_SUBRESOURCE ms;
			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
			s_devcon->Map(m_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
			return ms;
		}

		void unmap() { s_devcon->Unmap(m_pVBuffer, NULL); }

		void bind(uint32_t slot, uint32_t stride) { UINT flags[1] = { NULL }; s_devcon->IASetVertexBuffers(slot, 1, m_pVBuffer.access(), &stride, &flags[0]); }

		uint32_t size() { return m_size; }
		void clean() { m_pVBuffer.release(); }
	};

	class IndexBuffer
	{
		DxResPtr<ID3D11Buffer> m_pIBuffer;
	public:
		IndexBuffer() = default;

		void init(std::vector<Mesh::Triangle>& indices, D3D11_USAGE usage);
		void bind(); 

		D3D11_MAPPED_SUBRESOURCE& map();

		void unmap();

		void clean();
	};

	class Model
	{
	public:
		std::string name;

		struct MeshRange
		{
			uint32_t vertexOffset; // offset in vertices
			uint32_t indexOffset; // offset in indices
			uint32_t vertexNum; // num of vertices
			uint32_t indexNum; // num of indices
		};

	protected:
		bool indexed;
		std::vector<Mesh> meshes;
		std::vector<MeshRange> m_meshRange; // where meshes are stored
		VertexBuffer<Mesh::Vertex> m_vertices; // stores vertices of all meshes of this Model
		IndexBuffer m_indices; // stores vertex indices of all meshes of this Model

	public:
		Box box;
		Model() = default;

		void init(std::vector<Mesh::Vertex>& vertices, std::vector<Mesh::Triangle> indices, std::vector<Mesh>& meshes);
		void init(std::vector<Mesh::Vertex>& vertices, std::vector<Mesh>& meshes);

		Model& model() { return *this; }

		IndexBuffer& indexBuffer() { return m_indices; }
		VertexBuffer<Mesh::Vertex>& vertexBuffer() { return m_vertices; }
		MeshRange meshRange(std::size_t pos) { return m_meshRange[pos]; }

		void initMeshes(std::vector<Mesh>& meshes) { this->meshes = std::move(meshes); }
		void addMeshRange(MeshRange range) { m_meshRange.push_back(range); }
		void updateTriangleOctrees();

		void clean();
		
		friend class OpaqueInstances;
		friend class DissolutionInstances;
		friend class DisappearanceInstances;
		friend class EmissiveInstances;
		friend class MeshSystem;
		friend class ModelManager;
		friend class Terrain;
	};

}
