#include "Camera.h"
#include "Logger.h"

namespace Shard {

	Camera::Camera()
		:
		pos(glm::vec3(0.0f, 5.0f, 50.0f)),
		up(glm::vec3(0.0f, 1.0f, 0.0f)),
		front(glm::vec3(0.0f, 0.0f, -1.0f)),
		worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		yaw(YAW),
		pitch(PITCH),
		movementSpeed(SPEED),
		fov(45.f),
		player_game_obj(nullptr),
		status(FREE),
		player_obj_set(false),
		third_person_offset(glm::vec3(0)),
		first_person_offset(glm::vec3(0)),
		third_person_offset_set(false),
		first_person_offset_set(false)
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

	void Camera::setPlayerGameObj(std::shared_ptr<GameObject> game_obj) {
		player_game_obj = game_obj;
		player_obj_set = true;
	}

	void Camera::setFirstPersonOffset(glm::vec3 offset) {
		first_person_offset = offset;
		first_person_offset_set = true;
	}

	void Camera::setThirdPersonOffset(glm::vec3 offset, glm::vec3 look_at_offset){
		third_person_offset = offset;
		third_look_at_offset = look_at_offset;
		third_person_offset_set = true;
	}
	void Camera::updateCameraToPlayer() {
		if (status == CameraView::FREE || !player_obj_set) return;
		
		if (status == CameraView::FIRST_PERSON && !first_person_offset_set)
			Logger::log("First person offset not set", LoggerLevel::LOG_LEVEL_WARNING);
		if (status == CameraView::THIRD_PERSON && !third_person_offset_set)
			Logger::log("Third person offset not set", LoggerLevel::LOG_LEVEL_WARNING);

		auto playerPos = player_game_obj->m_model->position();
		auto offset = status == CameraView::FIRST_PERSON ? first_person_offset : third_person_offset;
		auto rotation = player_game_obj->m_model->getRotationMatrix();
		if (status == CameraView::LOCK) {
			front = normalize(playerPos - pos);
			right = normalize(glm::cross(front, up));
			up = glm::normalize(glm::cross(right, front));
			return;
		}
		pos = playerPos + rotation * offset;
		//TODO, this should not be - very bad!!!
		front = -player_game_obj->m_model->m_forward;
		up = player_game_obj->m_model->m_up;
		right = player_game_obj->m_model->m_right;
		if (status == CameraView::THIRD_PERSON) {
			front = glm::normalize((playerPos + rotation *third_look_at_offset) - pos);
			right = glm::normalize(glm::cross(front, up));
			up = glm::normalize(glm::cross(right, front));
		}

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