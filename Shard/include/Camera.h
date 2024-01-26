#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_transform.hpp>

namespace Shard {

	enum Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	class Camera {

	public:
		// Camera attributes
		glm::vec3 pos;
		glm::vec3 up;
		glm::vec3 front;
		glm::vec3 right;
		glm::vec3 worldUp;
		// Euler angles
		float yaw;
		float pitch;
		// Camera options
		float movementSpeed;
		
		SHARD_API Camera();
		
		SHARD_API glm::mat4 viewMatrix();
		SHARD_API void move(Movement direction, float deltaTime);
		SHARD_API void rotate(float delta_x, float delta_y);

	private:
		// Default camera values
		const float YAW = -90.0f;
		const float PITCH = 0.0f;
		const float SPEED = 10.0f;
		const float ROTATION_SPEED = 0.1f;
		const float SENSITIVITY = 0.1f;
	};
}