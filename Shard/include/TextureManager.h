#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "common.h"

namespace Shard {

	class TextureManager {
	public:
		static TextureManager& getInstance();

		TextureManager(TextureManager const&) = delete;
		TextureManager& operator=(TextureManager const&) = delete;

		void loadTexture(GLuint* target, const std::string& path);
		GLuint loadTexture(const std::string& path);
		GLuint getTexture(const std::string& path);

	private:
		TextureManager() = default;

		std::unordered_set<GLuint> _textures;
		std::unordered_map<std::string, GLuint> _texture_paths;

	};

}