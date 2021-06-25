#include "Shader.h"

#include <filesystem>

#include "Device.h"

#include <fstream>
#include <string>
#include <iostream>

#include "../path.h"

namespace Vulkan
{
	Shader::Shader(const Device& device, const std::string& filename):
		device(device)
	{
		auto root = Path::Root({"PBRVulkan", "Assets", "Shaders"});
		CreateShaderModule(Load((root / filename).string()));
	}

	Shader::~Shader()
	{
		if (shader != nullptr)
		{
			vkDestroyShaderModule(device.Get(), shader, nullptr);
			shader = nullptr;
		}
	}

	void Shader::CreateShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VK_CHECK(vkCreateShaderModule(device.Get(), &createInfo, nullptr, &shader), "Create shader");
	}

	VkPipelineShaderStageCreateInfo Shader::CreateShaderStage(VkShaderStageFlagBits stage) const
	{
		VkPipelineShaderStageCreateInfo shaderStageInfo{};
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.stage = stage;
		shaderStageInfo.module = shader;
		shaderStageInfo.pName = "main";

		return shaderStageInfo;
	}

	std::vector<char> Shader::Load(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			std::cout << "[ERROR] Failed to open file: '" + filename + "'" << std::endl;
		}

		std::cout << "[INFO] Shader loaded " << filename << std::endl;

		const auto fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
}
