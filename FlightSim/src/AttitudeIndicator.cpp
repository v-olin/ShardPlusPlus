#include "AttitudeIndicator.h"

#include "ShaderManager.h"
#include "TextureManager.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#define PI 3.14159265359

AttitudeIndicator::AttitudeIndicator(std::shared_ptr<Shard::Model> plane)
	: m_plane(plane)
	, m_size(100.f)
	, m_vao(SIZE_MAX)
{
	createGeometry();

	Shard::ShaderManager& sm = Shard::ShaderManager::getInstance();
	sm.loadShader("../FlightSim/shaders/", "attitudeGauge", false);

	Shard::TextureManager& tm = Shard::TextureManager::getInstance();
	m_background = tm.loadTextureRGBA("../FlightSim/textures/attitude_background.png");
	m_overlay = tm.loadTextureRGBA("../FlightSim/textures/attitude_overlay.png");
}

void AttitudeIndicator::createGeometry() {
	const float z = 2.f;
	const std::vector<glm::vec3> vertices{
		{ -m_size, -m_size, z },
		{ m_size, -m_size, z },
		{ m_size, m_size, z },
		{ -m_size, m_size, z },
	};

	const std::vector<int> indices{
		0, 1, 3,
		3, 1, 2
	};

	const std::vector<glm::vec2> uvCoords{
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 }
	};

	GLuint m_pbo = 0;
	GLuint m_uvbo = 0;
	GLuint m_ibo = 0;
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_pbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_pbo);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(glm::vec3),
		vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbo);
	glBufferData(GL_ARRAY_BUFFER,
		uvCoords.size() * sizeof(glm::vec2),
		uvCoords.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(int),
		indices.data(), GL_STATIC_DRAW);
}

void AttitudeIndicator::getPlaneAngles(float* pitch, float* roll) {
	glm::vec3 angles = m_plane->rotation();

	*pitch = angles.z;
	*roll = angles.x;

	if (*pitch > PI / 2) {
		*pitch = PI - *pitch;
		*roll += PI;
	}
	else if (*pitch < -PI / 2) {
		*pitch = *pitch + PI;
		*roll -= PI;
	}
}

void AttitudeIndicator::render() {
	const glm::vec2 size{ m_size * 2 };
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, size.x, size.y);
	glEnable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);
	glScissor(0, 0, size.x, size.y);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto sm = Shard::ShaderManager::getInstance();

	auto shader = sm.getShader("attitudeGauge");
	glUseProgram(shader);

	float pitch = 0.f;
	float roll = 0.f;
	getPlaneAngles(&pitch, &roll);

	sm.SetFloat1(shader, pitch, "planePitchRad");
	sm.SetFloat1(shader, roll, "planeRollRad");

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, m_overlay);
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, m_background);

	glm::mat4 viewMatrix = glm::lookAt(
		glm::vec3(0.f), // set camera at origin
		glm::vec3(0.f, 0.f, 1.f), // triangles are centered around the (0, 0, z) axis at +2.f z, so look at +z
		glm::vec3(0.f, 1.f, 0.f)
	);
	float xp = size.x / 2, yp = size.y / 2;
	glm::mat4 projMatrix = glm::ortho(-xp, xp, -yp, yp, 1.f, 3.f);

	sm.SetMat4x4(shader, projMatrix * viewMatrix, "projMatrix");

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glUseProgram(0);
}