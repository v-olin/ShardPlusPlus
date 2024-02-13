#include "Camera.h"

namespace Shard {
	Camera::Camera()
		:
		pos(glm::vec3(0.0f, 5.0f, 25.0f)),
		up(glm::vec3(0.0f, 1.0f, 0.0f)),
		front(glm::vec3(0.0f, 0.0f, -1.0f)),
		worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		yaw(YAW),
		pitch(PITCH),
		movementSpeed(SPEED)
	{
		right = glm::normalize(glm::cross(front, up));
	}

	glm::mat4 Camera::viewMatrix()
	{
		return glm::lookAt(pos, pos + front, up);
	}

	void Camera::move(Movement direction, float deltaTime)
	{
		float speed = movementSpeed * deltaTime;
		if (direction == FORWARD)
			pos = glm::translate(front * speed) * glm::vec4(pos, 1.0f);
		if (direction == BACKWARD)
			pos = glm::translate(-front * speed) * glm::vec4(pos, 1.0f);
		if (direction == RIGHT)
			pos = glm::translate(right * speed) * glm::vec4(pos, 1.0f);
		if (direction == LEFT)
			pos = glm::translate(-right * speed) * glm::vec4(pos, 1.0f);
		if (direction == UP)
			pos = glm::translate(worldUp * speed) * glm::vec4(pos, 1.0f);
		if (direction == DOWN)
			pos = glm::translate(-worldUp * speed) * glm::vec4(pos, 1.0f);
	}

	void Camera::rotate(float delta_x, float delta_y)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::mat4 yaw = glm::rotate(ROTATION_SPEED * -delta_x, worldUp);
		glm::mat4 pitch = glm::rotate(ROTATION_SPEED * delta_y, glm::normalize(glm::cross(front, worldUp)));
		front = glm::vec3(pitch * yaw * glm::vec4(front, 0.0f));

		// Recalculate UP and RIGHT vectors
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
}