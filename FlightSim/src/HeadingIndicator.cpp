#include "HeadingIndicator.h"

#include "ShaderManager.h"
#include "TextureManager.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm.hpp>
#include <gtx/projection.hpp>
#include <gtx/vector_angle.hpp>
#include <gtc/matrix_transform.hpp>

HeadingIndicator::HeadingIndicator(std::shared_ptr<Shard::Model> plane)
	: m_plane(plane)
	, m_size(100.f)
	, m_vao(SIZE_MAX)
{
	createGeometry();

	Shard::ShaderManager& sm = Shard::ShaderManager::getInstance();
	sm.loadShader("../FlightSim/shaders/", "headingGauge", false);

	Shard::TextureManager& tm = Shard::TextureManager::getInstance();
	m_background = tm.loadTextureRGBA("../FlightSim/textures/heading_background.png");
	m_overlay = tm.loadTextureRGBA("../FlightSim/textures/heading_overlay.png");
}

void HeadingIndicator::createGeometry() {
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

// IN RADIANS!!!
float HeadingIndicator::getPlaneHeading() {
	glm::vec3 dir = m_plane->m_forward;
	glm::vec3 projDir = glm::proj(dir, glm::normalize(glm::vec3{ 1.0f, 0.0f, 1.0f }));

	glm::vec2 refDir = glm::normalize(glm::vec2{ 1.0f, 0.0f });
	glm::vec2 dir2D = glm::normalize(glm::vec2{ dir.x, dir.z });

	auto angle = glm::orientedAngle(refDir, dir2D);

	return angle;
}

void HeadingIndicator::render() {
	const glm::vec2 size{ m_size * 2 };
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(size.x, 0, size.x, size.y);
	glEnable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);
	glScissor(size.x, 0, size.x, size.y);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto sm = Shard::ShaderManager::getInstance();

	auto shader = sm.getShader("headingGauge");
	glUseProgram(shader);

	float headingRad = getPlaneHeading();

	sm.SetFloat1(shader, headingRad, "planeHeadingRad");

	glActiveTexture(GL_TEXTURE17);
	glBindTexture(GL_TEXTURE_2D, m_overlay);
	glActiveTexture(GL_TEXTURE18);
	glBindTexture(GL_TEXTURE_2D, m_background);

	glm::mat4 viewMatrix = glm::lookAt(
		glm::vec3(0.f), // set camera at origin
		glm::vec3(0.f, 0.f, 1.f), // triangles are centered around the (0, 0, z) axis at +2.f z, so look at +z
		glm::vec3(0.f, 1.f, 0.f)
	);
	//glm::mat4 viewMatrix = m_sceneManager.getCameraViewMatrix();
	float xp = size.x / 2, yp = size.y / 2;
	glm::mat4 projMatrix = glm::ortho(-xp, xp, -yp, yp, 1.f, 3.f);

	sm.SetMat4x4(shader, projMatrix * viewMatrix, "projMatrix");

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glUseProgram(0);
}