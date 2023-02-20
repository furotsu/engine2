#include "assimpLoader.hpp"
#include "debug.hpp"
#include "transformSystem.hpp"

namespace assimp
{ 
void loadModel(std::string path, std::string filename, std::shared_ptr<engine::Model> model)
{
	// Load aiScene
	uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

	Assimp::Importer importer;
	const aiScene* assimpScene = importer.ReadFile(path + filename, flags);
	ASSERT(assimpScene);

	uint32_t numMeshes = assimpScene->mNumMeshes;

	// Load vertex data
	model->name = filename;
	std::vector<engine::Mesh> meshes(numMeshes);

	ASSERT(sizeof(XMFLOAT3) == sizeof(aiVector3D));
	ASSERT(sizeof(engine::Mesh::Triangle) == 3 * sizeof(uint32_t));

	engine::Model::MeshRange range;
	range.vertexOffset = 0u;
	range.indexOffset = 0u;
	range.vertexNum = 0u;
	range.indexNum = 0u;

	std::vector<engine::Mesh::Vertex> vertices;
	std::vector<engine::Mesh::Triangle> indices;

	for (uint32_t i = 0; i < numMeshes; ++i)
	{
		range.vertexOffset += range.vertexNum;
		range.indexOffset += range.indexNum;

		auto& srcMesh = assimpScene->mMeshes[i];
		auto& dstMesh = meshes[i];
		dstMesh.box = {};
		dstMesh.indexed = true;
		
		range.vertexNum = srcMesh->mNumVertices;
		range.indexNum = srcMesh->mNumFaces * 3;

		model->addMeshRange(range);

		dstMesh.name = srcMesh->mName.C_Str();
		dstMesh.box.min = reinterpret_cast<XMFLOAT3&>(srcMesh->mAABB.mMin);
		dstMesh.box.max = reinterpret_cast<XMFLOAT3&>(srcMesh->mAABB.mMax);

		dstMesh.vertices.resize(srcMesh->mNumVertices);
		dstMesh.triangles.resize(srcMesh->mNumFaces);

		XMMATRIX x = reinterpret_cast<XMMATRIX&>(assimpScene->mRootNode->mChildren[i]->mTransformation);
		dstMesh.m_transform = XMMatrixTranspose(x);
		dstMesh.m_transformInv = XMMatrixInverse(nullptr, dstMesh.m_transform);

		for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
		{
			engine::Mesh::Vertex& vertex = dstMesh.vertices[v];
			vertex.position = reinterpret_cast<XMFLOAT3&>(srcMesh->mVertices[v]);
			vertex.texCoords = reinterpret_cast<XMFLOAT2&>(srcMesh->mTextureCoords[0][v]);
			vertex.tangent = reinterpret_cast<XMFLOAT3&>(srcMesh->mTangents[v]);
			vertex.bitangent = reinterpret_cast<XMFLOAT3&>(srcMesh->mBitangents[v]);
			vertex.normal = reinterpret_cast<XMFLOAT3&>(srcMesh->mNormals[v]);

			vertices.push_back(vertex);
		}

		for (uint32_t f = 0; f < srcMesh->mNumFaces; ++f)
		{
			const auto& face = srcMesh->mFaces[f];
			DEV_ASSERT(face.mNumIndices == 3);
			dstMesh.triangles[f] = *reinterpret_cast<engine::Mesh::Triangle*>(face.mIndices);
			indices.push_back(dstMesh.triangles[f]);
		}
	}

	model->init(vertices, indices, meshes);

}
}