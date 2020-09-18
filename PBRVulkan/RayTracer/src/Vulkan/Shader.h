#pragma once

#include <string>
#include <vector>

#include "Vulkan.h"

namespace Vulkan
{
	/*
	 * Class for loading and creating shaders.
	 * Each shader has to be compiled to binary code before usage.
	 */
	class Shader final
	{
	public:
		NON_COPIABLE(Shader)

		Shader(const class Device& device, const std::string& filename);
		~Shader();

		[[nodiscard]] VkShaderModule Get() const { return shader; }
		VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits stage) const;

	private:
		static std::vector<char> Load(const std::string& filename);
		void CreateShaderModule(const std::vector<char>& code);

		const class Device& device;
		VkShaderModule shader{};
	};
}
