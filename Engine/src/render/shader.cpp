#include "shader.hpp"
#include "hlsl.hpp"
#include "d3d.hpp"

void engine::ShaderProgram::init(std::vector<ShaderInfo>& shaders)
{
	for (ShaderInfo& shader : shaders)
	{
		switch (shader.type)
		{
		case ShaderType::VERTEX:
		{
			compileShader(shader, VS);
			s_device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, m_pVS.access());
		}break;
		case ShaderType::PIXEL:
		{
			compileShader(shader, PS);
			s_device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, m_pPS.access());
		}break;
		case ShaderType::GEOMETRY:
		{
			compileShader(shader, GS);
			s_device->CreateGeometryShader(GS->GetBufferPointer(), GS->GetBufferSize(), NULL, m_pGS.access());
		}break;
		case ShaderType::COMPUTE:
		{
			compileShader(shader, CS);
			s_device->CreateComputeShader(CS->GetBufferPointer(), CS->GetBufferSize(), NULL, m_pCS.access());
		}break;
		default:
		{}
		}
	}
}

void engine::ShaderProgram::init(std::vector<ShaderInfo>& shaders, std::vector<D3D11_INPUT_ELEMENT_DESC>& ied)
{
	m_ied = ied;
	init(shaders);
	
	auto res = s_device->CreateInputLayout(m_ied.data(), ied.size(), VS->GetBufferPointer(), VS->GetBufferSize(), m_pLayout.access());
	ALWAYS_ASSERT(res == S_OK && "failed to create input layout for vertex shader" );

	s_devcon->IASetInputLayout(m_pLayout);
}

void engine::ShaderProgram::clean()
{
	for (auto& buffer : vertexUniformBuffers)
		buffer.release();
	for (auto& buffer : pixelUniformBuffers)
		buffer.release();
	for (auto& buffer : geometryUniformBuffers)
		buffer.release();
	for (auto& buffer : computeUniformBuffers)
		buffer.release();
	m_pVS.release();
	m_pPS.release();
	m_pGS.release();
	m_pCS.release();
	m_pLayout.release();
}

D3D11_MAPPED_SUBRESOURCE engine::ShaderProgram::mapUniformVertex(uint32_t slotOffset)
{
	D3D11_MAPPED_SUBRESOURCE res;
	s_devcon->Map(vertexUniformBuffers[slotOffset], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &res);
	return res;
}

D3D11_MAPPED_SUBRESOURCE engine::ShaderProgram::mapUniformPixel(uint32_t slotOffset)
{
	D3D11_MAPPED_SUBRESOURCE res;
	s_devcon->Map(pixelUniformBuffers[slotOffset], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &res);
	return res;
}

D3D11_MAPPED_SUBRESOURCE engine::ShaderProgram::mapUniformGeom(uint32_t slotOffset)
{
	D3D11_MAPPED_SUBRESOURCE res;
	s_devcon->Map(geometryUniformBuffers[slotOffset], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &res);
	return res;
}

D3D11_MAPPED_SUBRESOURCE engine::ShaderProgram::mapUniformCompute(uint32_t slotOffset)
{
	D3D11_MAPPED_SUBRESOURCE res;
	s_devcon->Map(computeUniformBuffers[slotOffset], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &res);
	return res;
}

void engine::ShaderProgram::unmapUniformVertex(uint32_t slotOffset)
{
	s_devcon->Unmap(vertexUniformBuffers[slotOffset], NULL);
}

void engine::ShaderProgram::unmapUniformPixel(uint32_t slotOffset)
{
	s_devcon->Unmap(pixelUniformBuffers[slotOffset], NULL);
}

void engine::ShaderProgram::unmapUniformGeom(uint32_t slotOffset)
{
	s_devcon->Unmap(geometryUniformBuffers[slotOffset], NULL);
}

void engine::ShaderProgram::unmapUniformCompute(uint32_t slotOffset)
{
	s_devcon->Unmap(computeUniformBuffers[slotOffset], NULL);
}

void engine::ShaderProgram::bindBufferVertex(uint32_t slotOffset, uint32_t bufferID)
{
	s_devcon->VSSetConstantBuffers(PER_OBJECT_UNIFORM_START_SLOT + slotOffset, 1, vertexUniformBuffers[bufferID].access());
}

void engine::ShaderProgram::bindBufferPixel(uint32_t slotOffset, uint32_t bufferID)
{
	s_devcon->PSSetConstantBuffers(PER_OBJECT_UNIFORM_START_SLOT + slotOffset, 1, pixelUniformBuffers[bufferID].access());
}

void engine::ShaderProgram::bindBufferGeom(uint32_t slotOffset, uint32_t bufferID)
{
	s_devcon->GSSetConstantBuffers(PER_OBJECT_UNIFORM_START_SLOT + slotOffset, 1, geometryUniformBuffers[bufferID].access());
}

void engine::ShaderProgram::bindBufferCompute(uint32_t slotOffset, uint32_t bufferID)
{
	s_devcon->CSSetConstantBuffers(PER_OBJECT_UNIFORM_START_SLOT + slotOffset, 1, computeUniformBuffers[bufferID].access());
}

uint32_t engine::ShaderProgram::createUniform(UINT size, ShaderType shaderType)
{
	uint32_t bufferID = 0;
	DxResPtr<ID3D11Buffer> pBuffer;

	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DYNAMIC;
	cbbd.ByteWidth = size;
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbbd.MiscFlags = 0;

	HRESULT hr = s_device->CreateBuffer(&cbbd, NULL, pBuffer.reset());
	
	ASSERT(!hr && "cannot create shader buffer");

	if (shaderType == ShaderType::VERTEX)
	{
		vertexUniformBuffers.push_back(pBuffer);
		m_vertexSizes.push_back(size);
		bufferID = vertexUniformBuffers.size() - 1;

	}
	else if (shaderType == ShaderType::PIXEL)
	{
		pixelUniformBuffers.push_back(pBuffer);
		m_pixelSizes.push_back(size);
		bufferID = pixelUniformBuffers.size() - 1;
	}
	else if (shaderType == ShaderType::GEOMETRY)
	{
		geometryUniformBuffers.push_back(pBuffer);
		m_geometrySizes.push_back(size);
		bufferID = geometryUniformBuffers.size() - 1;
	}
	else if (shaderType == ShaderType::COMPUTE)
	{
		computeUniformBuffers.push_back(pBuffer);
		m_computeSizes.push_back(size);
		bufferID = computeUniformBuffers.size() - 1;
	}
	else
		ASSERT(true && " unable to create uniform");
	return bufferID;
}

void engine::ShaderProgram::compileShader(const ShaderInfo& shader, ID3DBlob*& blob)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	LPCSTR profile;

	switch (shader.type)
	{
		case ShaderType::VERTEX: profile = "vs_5_0"; break;
		case ShaderType::PIXEL: profile = "ps_5_0"; break;
		case ShaderType::GEOMETRY: profile = "gs_5_0"; break;
		case ShaderType::COMPUTE: profile = "cs_5_0"; break;
		default: profile = "\0";  ERROR("cannot set geometry profile"); break;
	}

	ID3DBlob* errorBlob = nullptr;
	HRESULT result = D3DCompileFromFile(shader.filePath, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, shader.funcName, profile, flags, NULL, &blob, &errorBlob);

	if (result)
	{
		if (errorBlob)
		{
			std::cout << (char*)errorBlob->GetBufferPointer();
		}
		else
		{
			std::wstring wsrt(shader.filePath);
			std::cout << std::string(std::string(wsrt.begin(), wsrt.end())) + " compilation failed";
		}
	}
}

void engine::ShaderProgram::bind()
{
	s_devcon->IASetInputLayout(m_pLayout);
	s_devcon->VSSetShader(m_pVS, NULL, NULL);
	s_devcon->GSSetShader(m_pGS, NULL, NULL);
	s_devcon->PSSetShader(m_pPS, NULL, NULL);
	s_devcon->CSSetShader(m_pCS, NULL, NULL);
}

void engine::ShaderProgram::unbind()
{
	s_devcon->VSSetShader(NULL, NULL, NULL);
	s_devcon->GSSetShader(NULL, NULL, NULL);
	s_devcon->PSSetShader(NULL, NULL, NULL);
	s_devcon->CSSetShader(NULL, NULL, NULL);
}