#pragma once

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_transform.hpp>

#include "GameObject.h"

// Default camera values
const float SPEED = 10.0f;
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ROTATION_SPEED = 0.1f;
const float SENSITIVITY = 0.1f;

namespace Shard {

	enum Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};


	enum CameraView {
		FIRST_PERSON,
		THIRD_PERSON,
		LOCK,
		FREE
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
		float fov;
		CameraView status;
	

		void setPlayerGameObj(std::shared_ptr<GameObject> game_obj);
		void setFirstPersonOffset(glm::vec3 offset);
		void setThirdPersonOffset(glm::vec3 offset, glm::vec3 look_at_offset);
		
		Camera();
		
		glm::mat4 viewMatrix();
		void move(Movement direction, float deltaTime);
		void rotate(float delta_x, float delta_y);
		void updateCameraToPlayer();
	private:
		glm::vec3 third_person_offset;
		glm::vec3 third_look_at_offset;

		glm::vec3 first_person_offset;
		std::shared_ptr<GameObject> player_game_obj;

		bool third_person_offset_set;
		bool first_person_offset_set;
		bool player_obj_set;

	};
}