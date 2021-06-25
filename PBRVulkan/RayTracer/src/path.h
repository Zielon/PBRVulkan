#pragma once

#include <filesystem>

class Path
{
public:
	static std::filesystem::path Root(std::vector<std::string> segments)
	{
		auto root = std::filesystem::current_path();
		while (root.string().find("PBRVulkan") != std::string::npos)
			root = root.parent_path();

		root /= "PBRVulkan";

		for (const auto segment : segments)
			root /= segment;

		return root;
	}
};
