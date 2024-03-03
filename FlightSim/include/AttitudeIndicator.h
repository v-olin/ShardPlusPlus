#pragma once

#include <memory>
#include "Model.h"
#include "RenderableObject.h"

class AttitudeIndicator : public Shard::RenderableObject {
public:

	AttitudeIndicator(std::shared_ptr<Shard::Model> plane);

	void render();

private:
	const float m_size;
	std::shared_ptr<Shard::Model> m_plane;
	GLuint m_vao;

	GLuint m_overlay;
	GLuint m_background;

	void createGeometry();
	void getPlaneAngles(float *pitch, float *roll);
};