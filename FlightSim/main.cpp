#include "PhysicsManager.h"
#include "GameObject.h"
#include "Logger.h"
#include "Bootstrap.h"
#include "PlayerPlane.h"
#include "GameObjectManager.h"
#include "Pathtracer.h"
#include <iostream>
#include <memory>
#include <conio.h>
#include <math.h>
#include "main.h"

#include "SceneManager.h"

#undef main
#define FAST 100000000000;
#define SLOW 50;

int cameraStatus{ Shard::CameraView::FREE };
std::string cameraStatusStr[] = { "First person", "Third person", "Lock", "Free" };


bool is_rmb_down{ false };
bool is_first_mouse{ false };
float last_x{ 0.0f }, last_y{ 0.0f };

bool c_forward;
bool c_backward;
bool c_left;
bool c_right;
bool c_up;
bool c_down;

void GameTest::handleEvent(Shard::InputEvent ie, Shard::EventType et) {
	
	static auto &sm = Shard::SceneManager::getInstance();
	
	if (et == Shard::EventType::KeyDown)
	{
		if (ie.key == GLFW_KEY_SPACE)
			createBullet();
		if (ie.key == GLFW_KEY_R)
			sm.camera.setPlayerGameObj(playerPlane);
		if (ie.key == GLFW_KEY_F)
			playerPlane->m_drawCollider = !playerPlane->m_drawCollider;

	}
	if ((cameraStatus == Shard::CameraView::FREE || cameraStatus == Shard::CameraView::LOCK) && is_rmb_down) {
		auto keyDown = et == Shard::EventType::KeyDown;
		if (et == Shard::EventType::KeyDown || et == Shard::EventType::KeyUp) {
			if (ie.key == GLFW_KEY_W) 
				c_forward = keyDown;
			if (ie.key == GLFW_KEY_S) 
				c_backward = keyDown;
			if (ie.key == GLFW_KEY_A)
				c_left = keyDown;
			if (ie.key == GLFW_KEY_D)
				c_right = keyDown;
			if (ie.key == GLFW_KEY_LEFT_SHIFT)
				c_up = keyDown;
			if (ie.key == GLFW_KEY_LEFT_CONTROL)
				c_down = keyDown;
			if (keyDown && ie.key == GLFW_KEY_UP)
				sm.camera.movementSpeed += 100;
			if (keyDown && ie.key == GLFW_KEY_DOWN)
				sm.camera.movementSpeed -= 100;
		}
	}
	float xpos = static_cast<float>(ie.x);
	float ypos = static_cast<float>(ie.y);

	if (ie.button == GLFW_MOUSE_BUTTON_RIGHT && et == Shard::EventType::MouseUp) {
		is_rmb_down = false;
		is_first_mouse = true;
		c_forward = false;
		c_backward = false;
		c_left = false;
		c_right = false;
		c_up = false;
		c_down = false;
	}
	if (ie.button == GLFW_MOUSE_BUTTON_RIGHT && et == Shard::EventType::MouseDown) {
		is_rmb_down = true;
		last_x = xpos;
		last_y = ypos;
	}
	if (cameraStatus == Shard::CameraView::FREE) {
		if (et == Shard::EventType::MouseMotion){
			if (is_first_mouse) {
				last_x = xpos;
				last_y = ypos;
				is_first_mouse = false;
				return;
			}
			if (is_rmb_down) {
				float delta_x = xpos - last_x;
				float delta_y = last_y - ypos;
				last_x = xpos;
				last_y = ypos;
				float sensitivity = 0.05f;
				delta_x *= sensitivity;
				delta_y *= sensitivity;
				sm.camera.rotate(delta_x, delta_y);
				Shard::PathTracer::restart();
			}

		}
	
	}
	if (et == Shard::EventType::MouseDown && ie.button == GLFW_MOUSE_BUTTON_1) {
        if (ie.body != nullptr && ie.body->m_parent->hasTag("AIPlane")) {
			lockedTarget = ie.body->m_parent;
			for (auto plane : aiPlanes)
				plane->m_drawCollider = false;
			lockedTarget->m_drawCollider = true;
        }
	}

}

int GameTest::getTargetFrameRate() {
	return target_framerate;
}

void GameTest::update() {
	std::string second_fps = std::to_string(Shard::Bootstrap::getSecondFPS());
	std::string fps = std::to_string(Shard::Bootstrap::getFPS());

	static auto &sm = Shard::SceneManager::getInstance();
	sm.camera.setFirstPersonOffset(fp_offset);
	auto dt = Shard::Bootstrap::getDeltaTime();
	if(c_forward)
		sm.camera.move(Shard::Movement::FORWARD, dt);
	if(c_backward)
		sm.camera.move(Shard::Movement::BACKWARD, dt);
	if(c_left)
		sm.camera.move(Shard::Movement::LEFT, dt);
	if(c_right)
		sm.camera.move(Shard::Movement::RIGHT, dt);
	if(c_up)
		sm.camera.move(Shard::Movement::UP, dt);
	if(c_down)
		sm.camera.move(Shard::Movement::DOWN, dt);

	if (sm.camera.player_game_obj->m_toBeDestroyed) {
		cameraStatus = Shard::CameraView::THIRD_PERSON;
		sm.camera.setPlayerGameObj(playerPlane);
	}
	sm.camera.status = (Shard::CameraView)cameraStatus;


	if (c_forward || c_backward || c_left || c_right || c_up || c_down)
		Shard::PathTracer::restart();

	sm.camera.updateCameraToPlayer();

	Shard::Bootstrap::setUsePathTracing(usePathTracing);
	Shard::Bootstrap::setEnvironmentVariable("physics_debug", drawColliders ? "1" : "0");


	if(lockedTarget != nullptr && !lockedTarget->m_toBeDestroyed)
		lockedTarget->m_body->m_debugColor = { 1, 0, 0 };

	static int oldSampling = 0;
	if (oldSampling != pathTracingSampling){
		Shard::PathTracer::settings.subsampling = pathTracingSampling; 
		Shard::PathTracer::restart();
		oldSampling = pathTracingSampling;
	}
}

void GameTest::createPlayerPlane() {
	playerPlane = std::make_shared<PlayerPlane>();
	playerPlane->initialize();
	playerPlane->m_body->recalculateColliders();
	playerPlane->m_model->translate({ 0, 1000, 0 });
	playerPlane->m_model->scale({ 5, 5, 5 });
	playerPlane->rmb_down = &is_rmb_down;
	Shard::Bootstrap::getInput().addListeners(playerPlane);
	static auto &sm = Shard::SceneManager::getInstance();
	sm.camera.setPlayerGameObj(playerPlane);
	sm.camera.setFirstPersonOffset(glm::vec3(9, 8, 0));
	sm.camera.setThirdPersonOffset(glm::vec3(-50, 25, 0), glm::vec3(0, 15, 0));
	Shard::Bootstrap::setPlane(playerPlane->m_model);
}

void GameTest::createAIPlane(float x, float y, float z) {
	auto aiPlane = std::make_shared<AIPlane>();
	aiPlane->m_model = std::make_shared<Shard::Model>(AI_parent);
	aiPlane->initialize();
	aiPlane->m_model->translate({ x, y, z });
	float scale = 5;
	aiPlane->m_model->scale({ scale, scale, scale });
	aiPlane->m_body->recalculateColliders();

	Shard::Bootstrap::getInput().addListeners(aiPlane);

	aiPlanes.push_back(aiPlane);
}

void GameTest::createBullet() {
	auto bullet = std::make_shared<Bullet>();
	bullet->m_model = std::make_shared<Shard::Model>(bullet_parent);
	bullet->initialize();
	bullet->m_model->scale(glm::vec3(.5f));
	bullet->m_model->translate(playerPlane->m_model->position());
	bullet->m_model->m_rotMatrix = playerPlane->m_model->getRotationMatrix();
	//bullet->m_model->m_forward = playerPlane->m_model->m_forward;
	//bullet->m_model->m_up = playerPlane->m_model->m_up;
	//bullet->m_model->m_right = playerPlane->m_model->m_right;
	//bullet->m_model->translate({ 0,0,0 });

	//bullet->m_model->rotate(180, { 0, 1, 0 });
	bullet->m_body->recalculateColliders();
	bullet->lockedTarget = lockedTarget;

	static auto &sm = Shard::SceneManager::getInstance();
	if (lockedTarget != nullptr && !lockedTarget->m_toBeDestroyed) {
		cameraStatus = Shard::CameraView::THIRD_PERSON;
		sm.camera.setPlayerGameObj(bullet);
	}
	bullets.push_back(bullet);
}


float randf() {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void GameTest::initalize() {
	Shard::Logger::log("Initializing game");

	createPlayerPlane();
	planeAI = std::make_shared<AttitudeIndicator>(playerPlane->m_model);
	planeHI = std::make_shared<HeadingIndicator>(playerPlane->m_model);
	Shard::Bootstrap::addRenderObject(planeAI);
	Shard::Bootstrap::addRenderObject(planeHI);
	
	Shard::Bootstrap::getInput().addListeners(shared_from_this());
	AI_parent = std::make_shared<Shard::Model>("models/chopper.obj");
	bullet_parent = std::make_shared<Shard::Model>("models/AIM120D.obj");
	int max = 500;
	for (int i = 0; i < 0; i++) {
		auto pos = glm::vec3(rand() % max, rand() % max, rand() % max) - glm::vec3(max/2, 0, max/2);
		createAIPlane(pos.x, pos.y, pos.z);
	}

	
	
	static auto &sm = Shard::SceneManager::getInstance();
	
	drawColliders = Shard::Bootstrap::getEnvironmentVariable("physics_debug") == "1";
	
	Shard::Bootstrap::gui->addRadioSelector({ "View",  
		&cameraStatus,
		{
		std::make_tuple("First person", Shard::CameraView::FIRST_PERSON),
		std::make_tuple("Third person", Shard::CameraView::THIRD_PERSON),
		std::make_tuple("Lock camera on Obj", Shard::CameraView::LOCK),
		std::make_tuple("Free Cam", Shard::CameraView::FREE),
		} 
	});

	Shard::Bootstrap::gui->addCheckBox("Use Path Tracing", &usePathTracing);
	Shard::Bootstrap::gui->addCheckBox("Draw colliders", &drawColliders);
	Shard::Bootstrap::gui->addIntSlider("Path tracer sampling", &pathTracingSampling, 1, 16);
	Shard::Bootstrap::gui->addIntSlider("Framerate (0 is uncapped)", &target_framerate, 0, 999);

	Shard::Bootstrap::gui->addFloatSlider("First persoin offset x", &fp_offset.x, -10, 10);
	Shard::Bootstrap::gui->addFloatSlider("First persoin offset y", &fp_offset.y, -10, 10);
	Shard::Bootstrap::gui->addFloatSlider("First persoin offset z", &fp_offset.z, -10, 10);
}



int main() {
	Shard::Logger::log("Hello from game?!?!?");

	Shard::Logger::log("Creating 'GameTest' object");

	auto game = std::make_shared<GameTest>();
	Shard::Bootstrap::setRunningGame(game);
	

	Shard::Logger::log("Runnning Bootstrap::Main");
	Shard::Bootstrap::Main({});

	Shard::Logger::log("Exited out of Bootstrap::Main. Exiting program.");
	return 0;

}
