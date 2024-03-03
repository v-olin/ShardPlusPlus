#pragma once

#include <memory>
#include "Model.h"
#include "RenderableObject.h"

class HeadingIndicator : public Shard::RenderableObject {
public:
	HeadingIndicator(std::shared_ptr<Shard::Model> plane);

	void render();

private:
	const float m_size;
	std::shared_ptr<Shard::Model> m_plane{ nullptr };
	GLuint m_vao;

	GLuint m_overlay;
	GLuint m_background;

	void createGeometry();
	float getPlaneHeading(); // RADIANS!!!!
};