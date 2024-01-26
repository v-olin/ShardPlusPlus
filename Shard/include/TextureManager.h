#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <GL/glew.h>

namespace Shard {

	class TextureManager {
	public:
		TextureManager& getInstance();

		SHARD_API TextureManager(TextureManager const&) = delete;
		SHARD_API TextureManager& operator=(TextureManager const&) = delete;

		SHARD_API void loadTexture(GLuint* target, const std::string& path);
		SHARD_API GLuint loadTexture(const std::string& path);
		SHARD_API GLuint getTexture(const std::string& path);

	private:
		TextureManager();

		std::unordered_set<GLuint> _textures;
		std::unordered_map<const std::string&, GLuint> _texture_paths;

	};

}