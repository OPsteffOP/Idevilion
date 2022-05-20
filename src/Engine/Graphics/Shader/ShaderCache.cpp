// Folder: Graphics/Shader

#include "EnginePCH.h"
#include "ShaderCache.h"

#include "ByteStream.h"

ShaderCache* ShaderCache::GetInstance()
{
	static ShaderCache* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new ShaderCache();
	}

	return pInstance;
}

CompiledShaderData ShaderCache::LoadShader(const std::string& shaderPath, const std::vector<std::string>& defines, ShaderBackend backend)
{
	CompiledShaderData data;

	const std::string compiledShaderPath = GetCompiledShaderPath(shaderPath, defines, backend);
	if (!std::filesystem::exists(compiledShaderPath))
		return data;

	ByteStream input(new ByteStreamBinaryFileImpl(compiledShaderPath));

	const uint64_t expectedFileSize = input.ReadUInt64();
	const uint64_t expectedModifiedTime = input.ReadUInt64();

	if (std::filesystem::exists(shaderPath))
	{
		const uint64_t currentFileSize = std::filesystem::file_size(shaderPath);
		const uint64_t currentModifiedTime = std::chrono::duration_cast<std::chrono::duration<uint64_t>>(std::filesystem::last_write_time(shaderPath).time_since_epoch()).count();

		if (expectedFileSize != currentFileSize || expectedModifiedTime != currentModifiedTime)
			return data;
	}

	data.size = input.ReadUInt64();
	data.pBuffer = new char[data.size];
	input.ReadData(data.pBuffer, data.size, Endian::BIG_ENDIAN);

	return data;
}

void ShaderCache::StoreShader(const CompiledShaderData& data, const std::string& shaderPath, const std::vector<std::string>& defines, ShaderBackend backend)
{
	if (!std::filesystem::exists(shaderPath) || data.pBuffer == nullptr)
		return;

	const std::string compiledShaderPath = GetCompiledShaderPath(shaderPath, defines, backend);

	const uint64_t currentFileSize = std::filesystem::file_size(shaderPath);
	const uint64_t currentModifiedTime = std::chrono::duration_cast<std::chrono::duration<uint64_t>>(std::filesystem::last_write_time(shaderPath).time_since_epoch()).count();

	ByteStream output(new ByteStreamBinaryFileImpl(compiledShaderPath, true));

	output.WriteUInt64(currentFileSize);
	output.WriteUInt64(currentModifiedTime);

	output.WriteUInt64(data.size);
	output.WriteData(data.pBuffer, data.size, Endian::BIG_ENDIAN);
}

std::string ShaderCache::GetCompiledShaderPath(const std::string& shaderPath, const std::vector<std::string>& defines, ShaderBackend backend)
{
	size_t definesHash = 0; // TODO: this can still return duplicates, unlikely but possible
	for (const std::string& define : defines)
	{
		Utils::HashCombine(definesHash, define);
	}

	return std::filesystem::path(Paths::Data::SHADER_CACHE_DIR).append(ShaderBackendHelper::ToString(backend) + "/" +
		std::filesystem::path(shaderPath).stem().string() + "_" + std::to_string(definesHash) + Paths::Data::Extensions::COMPILED_SHADER).string();
}