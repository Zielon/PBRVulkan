#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../3rdParty/HDRLoader.h"

namespace Assets
{
	/*
	 * Create an empty small texture
	 */
	Texture::Texture(): texWidth(32), texHeight(32), texChannels(4), imageSize(texHeight * texWidth * texChannels)
	{
		pixels = new unsigned char[texHeight * texWidth * texChannels];
	}

	Texture::Texture(int width, int height, int channel, void* pixels)
		: isHDR(true), pixels(pixels), texWidth(width), texHeight(height),
		  texChannels(channel), imageSize(texHeight * texWidth * texChannels) { }

	Texture::Texture(const std::string& path): path(path)
	{
		pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		imageSize = texHeight * texWidth * 4;
		isHDR = false;

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
		if (pixels != nullptr && !isHDR)
		{
			stbi_image_free(pixels);
			pixels = nullptr;
		}
	}
}
