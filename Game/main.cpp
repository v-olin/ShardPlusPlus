#include "PhysicsManager.h"
#include "GameObject.h"
#include "Logger.h"
#include "Bootstrap.h"
#include "Car.h"
#include "Asteriod.h"
#include "GameObjectManager.h"
#include <iostream>
#include <memory>
#include <conio.h>
#include "main.h"

#include "SceneManager.h"

#undef main
#define FAST 100000000000;
#define SLOW 50;

Shard::CameraView cameraStatus{ Shard::CameraView::FREE };
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
		if (ie.key == GLFW_KEY_1)
			sm.camera.status = Shard::CameraView::FIRST_PERSON;
		if (ie.key == GLFW_KEY_2)
			sm.camera.status = Shard::CameraView::THIRD_PERSON;
		if (ie.key == GLFW_KEY_3)
			sm.camera.status = Shard::CameraView::FREE;
		if (ie.key == GLFW_KEY_4)
			sm.camera.status = Shard::CameraView::LOCK;
		if (ie.key == GLFW_KEY_5)
			sm.camera.setPlayerGameObj(asteroids[0]);
		if (ie.key == GLFW_KEY_6)
			sm.camera.setPlayerGameObj(car);


		if (cameraStatus != sm.camera.status) {
			cameraStatus = sm.camera.status;
			Shard::Logger::log(("Camera status: " + cameraStatusStr[(int)cameraStatus]).c_str());
		}
	}
	car->should_move = !is_rmb_down;// cameraStatus != Shard::CameraView::FREE;
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
				sm.camera.movementSpeed += 10;
			if (keyDown && ie.key == GLFW_KEY_DOWN)
				sm.camera.movementSpeed -= 10;
		}
	}


	//MouseCallback(GLFWwindow * window, double xpos, double ypos)
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
			}

		}
	
	}

}
int GameTest::getTargetFrameRate() {
	return 500	;
}

void GameTest::update() {
	std::string second_fps = std::to_string(Shard::Bootstrap::getSecondFPS());
	std::string fps = std::to_string(Shard::Bootstrap::getFPS());

	static auto &sm = Shard::SceneManager::getInstance();
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



	sm.camera.updateCameraToPlayer();


	/*Shard::Display* display = Shard::Bootstrap::getDisplay();
	display->showText(("FPS: " + second_fps + " / " + fps).c_str(), 10, 10, 12, 255, 255, 255);*/
}

void GameTest::createCar() {
	car = std::make_shared<Car>();
	car->initialize();
	car->m_body->recalculateColliders();
	Shard::Bootstrap::getInput().addListeners(car);

	static auto &sm = Shard::SceneManager::getInstance();
	sm.camera.setPlayerGameObj(car);
	sm.camera.setFirstPersonOffset(glm::vec3(-9, 8, 0));
	sm.camera.setThirdPersonOffset(glm::vec3(50, 25, 0), glm::vec3(0, 15, 0));
}

void GameTest::createAsteroid(float x, float y, float z) {
		
	auto asteroid = std::make_shared<Asteroid>();
	asteroid->initialize();
	asteroid->m_model->translate({ x, y, z });
	asteroid->m_model->scale({ 10.0f, 10.0f, 10.0f });
	asteroid->m_body->recalculateColliders();

    Shard::Bootstrap::getInput().addListeners(asteroid);
	asteroids.push_back(asteroid);

}

void GameTest::createFlatPlane(float x, float y, float z) {

	auto asteroid = std::make_shared<Asteroid>();
	asteroid->initialize();
	asteroid->m_model->translate({ x, y, z });
	asteroid->m_model->scale({ 250.0f, 1.0f, 250.0f });
	asteroid->m_body->recalculateColliders();
	asteroids.push_back(asteroid);

}

void GameTest::initalize() {
	Shard::Logger::log("Initializing game");
	createCar();
	createAsteroid(10, 0, -10);
	//createFlatPlane(0, -12, 0);
	Shard::Bootstrap::getInput().addListeners(shared_from_this());
	static auto &sm = Shard::SceneManager::getInstance();
	sm.camera.movementSpeed = 100;

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

/*
#include "PhysicsManager.h"
#include "GameObject.h"
#include "Logger.h"

#include <iostream>
#include <memory>
#include <conio.h>

#undef main

auto& manager = Shard::PhysicsManager::getInstance();

class GOB : public Shard::GameObject, public Shard::CollisionHandler
{
public:
	
	GOB()
	: Shard::GameObject()
	{
		// EMPTY CONSTRUCTOREEEEEEEEEEEEEEEEEE
	}
	void initialize(glm::vec3 initial_position) {
		setPhysicsEnabled();
		body_->trans->translate(initial_position);
		body_->trans->scale({10.0f, 10.0f, 10.0f});
	}

	void onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) override {
		Shard::Logger::log("COLLSION");
	}
    
	void onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) override {
		Shard::Logger::log("Exit colision");
	};
    void onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) override {
		Shard::Logger::log("stay colision");
	};
	void checkDestroyMe() override {};
	void initialize()  override {};
	void update()  override {};
	void physicsUpdate()  override {};
	void prePhysicsUpdate()  override {};
	void killMe()  override {};

private:
};

std::shared_ptr<GOB> CreateGOB(glm::vec3 initial_position) {
	std::shared_ptr<GOB> gob = std::make_shared<GOB>();
	gob->initialize(initial_position);
	return gob;
}

// Test 1: Spawn two game objects far away from each other.
// Expectation: We should not register any collision between them.
int Test1() {
	int passed{0};

	auto gob1 = CreateGOB({ 5.0f, 10.0f, 10.0f });
	auto gob2 = CreateGOB({ 50.0f, 10.0f, 10.0f });

	// Tick
	manager.update();

	// Check collisions
	if (manager.collision_last_frame == 0)
		passed = 1;

	manager.removePhysicsObject(gob1->body_);
	manager.removePhysicsObject(gob2->body_);
	// Result?
	std::cout << "Result test 1: " << std::to_string(passed == 1) << "\n";
	return passed;
}

// Test 2: Spawn two game objects close to each other such that they collide.
// Expectation: We should not register any collision between them.
int Test2() {
	int passed{0};

	auto gob1 = CreateGOB({ 45.0f, 5.0f, 5.0f });
	auto gob2 = CreateGOB({ 50.0f, 10.0f, 10.0f });

	// Tick
	manager.update();

	// Check collisions
	if (manager.collision_last_frame == 1 )
		passed = 1;

	manager.removePhysicsObject(gob1->body_);
	manager.removePhysicsObject(gob2->body_);
	// Result?
	std::cout << "Result test 2: " << std::to_string(passed == 1) << "\n";
	return passed;
}

int Test3() {
	int passed{ 0 };

	auto gob1 = CreateGOB({ 41.0f, 48.0f, 48.0f });
	//gob1->body_->pass_through = true;
	auto gob2 = CreateGOB({ 50.0f, 50.0f, 50.0f });

	// Tick
	manager.update();
	auto col_after_first = manager.collision_last_frame;
	manager.update();
	auto col_after_sec = manager.collision_last_frame;
	manager.update();

	// Check collisions
	if (col_after_sec == 0 && col_after_first == 1)
		passed = 1;

	manager.removePhysicsObject(gob1->body_);
	manager.removePhysicsObject(gob2->body_);
	// Result?
	std::cout << "Result test 3: " << std::to_string(passed == 1) << "\n";
	return passed;

}

int main() {

	std::cout << "Testing shit... 0 means it failed and 1 means it passed.\n\n";
	
	//int t1 = Test1(); // Should fail, no collision.
	//int t2 = Test2(); // Should pass, expecting collision.
	int t3 = Test3(); // Should pass, expecting collision.
	
	std::cout << "\nhaha, you suck! your tests failed lol (safe assumption being made here)\n";
	std::cin;

	// Everything will always be fine.
	return 0;

}
*/