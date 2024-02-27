#pragma once
#include <string>

#include "stb_image.h"
#include "glm.hpp"

#include "common.h"
#include "Logger.h"
namespace Shard {
	struct HDRImage
	{
		int width, height, components;
		float* data = nullptr;
		// Constructor
		HDRImage() {};

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
		glm::vec3 sample(float u, float v);
		void load(const std::string& filename);

	};
}
