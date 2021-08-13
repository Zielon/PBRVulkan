#pragma once

#include <filesystem>

class Path
{
public:
	static std::filesystem::path Root(std::vector<std::string> segments)
	{
		auto root = std::filesystem::current_path();
		auto current = root.filename();
		while (current.string().find("PBRVulkan") == std::string::npos)
		{
			root = root.parent_path();
			current = root.filename();
		}

		root = root.parent_path();

		for (const auto segment : segments)
			root /= segment;

		return root;
	}
};