#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Assets
{
	Texture::Texture(const std::string& path): path(path)
	{
		pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		imageSize = texHeight * texWidth * 4;

		if (!pixels)
		{
			throw std::runtime_error("Failed to load texture image!");
		}
	}

	Texture::Texture(Texture&& texture) noexcept
	{
		path = std::move(texture.path);
		texHeight = texture.texHeight;
		texWidth = texture.texWidth;
		texChannels = texture.texChannels;
		imageSize = texture.imageSize;
		pixels = texture.pixels;

		texture.pixels = nullptr;
	}

	Texture& Texture::operator=(Texture&& texture) noexcept
	{
		if (this != &texture)
		{
			path = std::move(texture.path);
			texHeight = texture.texHeight;
			texWidth = texture.texWidth;
			texChannels = texture.texChannels;
			imageSize = texture.imageSize;
			pixels = texture.pixels;

			texture.pixels = nullptr;
		}

		return *this;
	}

	Texture::~Texture()
	{
		stbi_image_free(pixels);
		pixels = nullptr;
	}
}
