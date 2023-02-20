#pragma once

#include <vector>
#include <filesystem>
#include "d3d.h"
#include "DxRes.hpp"


namespace fs = std::filesystem;

class ShaderIncluder : public ID3DInclude
{
public:
	ShaderIncluder(const fs::path& currentDir) : m_currentDir(currentDir)
	{
	}

	HRESULT Open(D3D_INCLUDE_TYPE includeType, const char* relativePath, LPCVOID pParentData, const void** outData, uint32_t* outBytes) override;
	HRESULT Close(const void* pData) override;

private:
	fs::path m_currentDir;
	std::vector<uint8_t> m_includeLevels;

	using FileData = std::vector<char>;
	std::vector<FileData> m_openedFiles;
};

HRESULT ShaderIncluder::Open(D3D_INCLUDE_TYPE includeType, const char* includePathStr, LPCVOID parentData, const void** outData, uint32_t* outBytes)
{
	fs::path includePath = includePathStr;

	uint8_t includeLevel = 0;
	fs::path tmpPath = includePath;
	while (tmpPath.has_parent_path())
	{
		tmpPath = tmpPath.parent_path();
		includeLevel += 1;
	}
	m_includeLevels.push_back(includeLevel);

	m_currentDir = m_currentDir / includePath;

	FileData& fileData = m_openedFiles.emplace_back();
	FileSystem::readFile(fileData, m_currentDir);
	*outData = fileData.data();
	*outBytes = uint32_t(fileData.size());

	m_currentDir = m_currentDir.parent_path();

	return *outBytes > 0 ? S_OK : E_FAIL;
}

HRESULT ShaderIncluder::Close(const void* data)
{
	for (uint8_t i = 0; i < m_includeLevels.back(); ++i)
		m_currentDir = m_currentDir.parent_path();

	m_includeLevels.pop_back();
	m_openedFiles.pop_back();

	return S_OK;
}