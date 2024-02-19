#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <filesystem>

#include "stb_image.h"

#include "common.h"
#include "Logger.h"

namespace Shard {
	struct HDRImage
	{
		int width, height, components;
		float* data = nullptr;
		// Constructor
		HDRImage(const std::string& filename)
		{
			stbi_set_flip_vertically_on_load(true);
			data = stbi_loadf(filename.c_str(), &width, &height, &components, 3);
			if (data == nullptr)
			{
				Logger::log("Failed to liad image: " + filename, LOG_LEVEL_FATAL);
			}
		};
		~HDRImage()
		{
			stbi_image_free(data);
		};
	};


	class TextureManager {
	public:
		static TextureManager& getInstance();

		TextureManager(TextureManager const&) = delete;
		TextureManager& operator=(TextureManager const&) = delete;

		void loadTexture(GLuint* target, const std::string& path);
		GLuint loadTexture(const std::string& path);
		GLuint getTexture(const std::string& path);
		GLuint loadHdrTexture(const std::string& filename);

	private:
		TextureManager() = default;

		std::unordered_set<GLuint> _textures;
		std::unordered_map<std::string, GLuint> _texture_paths;

	};

}