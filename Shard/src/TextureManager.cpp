#include "TextureManager.h"
#include "Logger.h"
#include "HDRImage.h"

#include <assert.h>

#include <stb_image.h>

namespace Shard {
	TextureManager& TextureManager::getInstance() {
		static TextureManager instance;
		return instance;
	}

	void TextureManager::loadTexture(GLuint* target, const std::string& path) {
		int w, h, c;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 3);

		if (data == nullptr) {
			std::string msg = "Couldn't load texture: " + path;
			Logger::log(msg.c_str(), LOG_LEVEL_ERROR);
			return;
		}

		glGenTextures(1, target);

		glBindTexture(GL_TEXTURE_2D, *target);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // plain RGB
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		// TODO:
		//_texture_paths[path] = *target;
	}

	GLuint TextureManager::loadTexture(const std::string& path) {
		int w, h, c;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 3);

		if (data == nullptr) {
			std::string msg = "Couldn't load texture: " + path;
			Logger::log(msg.c_str(), LOG_LEVEL_ERROR);
			return UINT32_MAX;
		}

		GLuint textureID;
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // plain RGB
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		_texture_paths[path] = textureID;

		return textureID;
	}

	GLuint TextureManager::loadTextureRGBA(const std::string& path) {
		int w, h, c;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 4);

		if (data == nullptr) {
			std::string msg = "Couldn't load texture: " + path;
			Logger::log(msg.c_str(), LOG_LEVEL_ERROR);
			return UINT32_MAX;
		}

		GLuint textureID;
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // plain RGBA
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		_texture_paths[path] = textureID;

		return textureID;
	}

	GLuint TextureManager::getTexture(const std::string& path) {
		if (_texture_paths.find(path) != _texture_paths.end())
			return _texture_paths[path];

		return loadTexture(path);
	}
	GLuint TextureManager::loadHdrTexture(const std::string& filename)
	{

		auto full_filename = "../Shard/res/envmaps/" + filename;
		GLuint texId;
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		HDRImage image(full_filename);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image.width, image.height, 0, GL_RGB, GL_FLOAT, image.data);

		return texId;
	}

}