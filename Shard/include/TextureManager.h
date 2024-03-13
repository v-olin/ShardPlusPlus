#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <filesystem>

#include "stb_image.h"
#include "glm.hpp"

#include "common.h"
#include "Logger.h"

namespace Shard {
	class TextureManager {
	public:
		static TextureManager& getInstance() {
			static TextureManager instance;
			return instance;
		}

		TextureManager(TextureManager const&) = delete;
		TextureManager& operator=(TextureManager const&) = delete;

		void loadTexture(GLuint* target, const std::string& path);
		GLuint loadTexture(const std::string& path);
		GLuint loadTextureRGBA(const std::string& path);
		GLuint getTexture(const std::string& path);
		GLuint loadHdrTexture(const std::string& filename);
		GLuint loadHdrMipMap(const std::vector<std::string> filenames);

	private:
		TextureManager() = default;

		std::unordered_set<GLuint> _textures;
		std::unordered_map<std::string, GLuint> _texture_paths;

	};

}