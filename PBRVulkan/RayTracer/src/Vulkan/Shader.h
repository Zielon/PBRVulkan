#pragma once

#include <string>
#include <vector>

#include "Vulkan.h"

namespace Vulkan
{
	class Shader final
	{
	public:
		NON_COPIABLE(Shader)

		Shader(const class Device& device, const std::string& filename);
		~Shader();

		VkShaderModule Get() const { return shader; }

		VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits stage) const;

	private:
		static std::vector<char> Load(const std::string& filename);
		void CreateShaderModule(const std::vector<char>& code);

		const class Device& device;
		VkShaderModule shader{};
	};
}
