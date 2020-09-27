#pragma once

#include <string>

#include "../Vulkan/Vulkan.h"

namespace Assets
{
	class Texture
	{
	public:
		Texture(const Texture&) = delete;
		Texture& operator =(const Texture&) = delete;
		Texture(const std::string& path);
		Texture(Texture&&) noexcept;
		Texture& operator =(Texture&&) noexcept;
		~Texture();

		[[nodiscard]] int GetWidth() const
		{
			return texWidth;
		};

		[[nodiscard]] int GetHeight() const
		{
			return texHeight;
		};

		[[nodiscard]] int GetImageSize() const
		{
			return imageSize;
		};

		[[nodiscard]] int GetChannels() const
		{
			return texChannels;
		};

		[[nodiscard]] const void* GetPixels() const
		{
			return pixels;
		};

	private:
		void* pixels;
		int texWidth{};
		int texHeight{};
		int texChannels{};
		int imageSize;
	};
}
