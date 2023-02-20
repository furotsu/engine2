#pragma once

#include <string>
#include <vector>

#include "debug.hpp"
#include"DxRes.hpp"

enum class ShaderType { VERTEX, PIXEL, GEOMETRY, COMPUTE};

struct ShaderInfo
{
	ShaderType type;
	LPCWSTR filePath;
	LPCSTR funcName;
};

namespace engine
{
	class ShaderProgram
	{
	private:
		ID3DBlob* VS = nullptr;
		ID3DBlob* PS = nullptr;
		ID3DBlob* GS = nullptr;
		ID3DBlob* CS = nullptr;
		DxResPtr<ID3D11VertexShader> m_pVS;
		DxResPtr<ID3D11PixelShader> m_pPS;
		DxResPtr<ID3D11GeometryShader> m_pGS;
		DxResPtr<ID3D11ComputeShader> m_pCS;
		DxResPtr<ID3D11InputLayout> m_pLayout = nullptr;   
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_ied;

	public:
		std::vector<DxResPtr<ID3D11Buffer>> vertexUniformBuffers;
		std::vector<DxResPtr<ID3D11Buffer>> pixelUniformBuffers;
		std::vector<DxResPtr<ID3D11Buffer>> geometryUniformBuffers;
		std::vector<DxResPtr<ID3D11Buffer>>	computeUniformBuffers;
		std::vector<UINT> m_vertexSizes;
		std::vector<UINT> m_pixelSizes;
		std::vector<UINT> m_geometrySizes;
		std::vector<UINT> m_computeSizes;

		ShaderProgram() = default;

		void init(std::vector<ShaderInfo> &shaders);
		void init(std::vector<ShaderInfo>& shaders, std::vector<D3D11_INPUT_ELEMENT_DESC>& ied);

		void clean();

		D3D11_MAPPED_SUBRESOURCE mapUniformVertex(uint32_t slotOffset = 0u);
		D3D11_MAPPED_SUBRESOURCE mapUniformPixel(uint32_t slotOffset = 0u);
		D3D11_MAPPED_SUBRESOURCE mapUniformGeom(uint32_t slotOffset = 0u);
		D3D11_MAPPED_SUBRESOURCE mapUniformCompute(uint32_t slotOffset = 0u);
		void unmapUniformVertex(uint32_t slotOffset = 0u);
		void unmapUniformPixel(uint32_t slotOffset = 0u);
		void unmapUniformGeom(uint32_t slotOffset = 0u);
		void unmapUniformCompute(uint32_t slotOffset = 0u);


		void bindBufferVertex(uint32_t slotOffset, uint32_t bufferID);
		void bindBufferPixel(uint32_t slotOffset, uint32_t bufferID);
		void bindBufferGeom(uint32_t slotOffset, uint32_t bufferID);
		void bindBufferCompute(uint32_t slotOffset, uint32_t bufferID);

		uint32_t createUniform(UINT size, ShaderType = ShaderType::VERTEX);
		inline bool isUniformsEmpty() { return (vertexUniformBuffers.size() == 0) && (pixelUniformBuffers.size() == 0); }

		void compileShader(const ShaderInfo& shader, ID3DBlob*& blob);

		void bind();
		void unbind();
	};
}