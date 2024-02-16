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

bool active{ false };

void GameTest::handleEvent(Shard::InputEvent ie, Shard::EventType et) {

	Shard::Logger::log("Jasdjhasdjhasjhaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

	static auto &sm = Shard::SceneManager::getInstance();

	if (et == Shard::EventType::KeyDown)
	{
		if (ie.key == GLFW_KEY_TAB) {
			active = !active;
		}
	}

	if (active) {
		Shard::Logger::log(("active status: " + std::to_string(active)).c_str());
		if (ie.key == GLFW_KEY_W)
			sm.camera.move(Shard::Movement::FORWARD, 1.0f);

		if (ie.key == GLFW_KEY_S)
			sm.camera.move(Shard::Movement::BACKWARD, 1.0f);

		if (ie.key == GLFW_KEY_A)
			sm.camera.move(Shard::Movement::LEFT, 1.0f);

		if (ie.key == GLFW_KEY_D)
			sm.camera.move(Shard::Movement::RIGHT, 1.0f);

		if (ie.key == GLFW_KEY_LEFT_SHIFT)
			sm.camera.move(Shard::Movement::UP, 1.0f);

		if (ie.key == GLFW_KEY_RIGHT_SHIFT)
			sm.camera.move(Shard::Movement::DOWN, 1.0f);


	}

}
int GameTest::getTargetFrameRate() {
	return FAST	;
}

void GameTest::update() {
	std::string second_fps = std::to_string(Shard::Bootstrap::getSecondFPS());
	std::string fps = std::to_string(Shard::Bootstrap::getFPS());

	/*Shard::Display* display = Shard::Bootstrap::getDisplay();
	display->showText(("FPS: " + second_fps + " / " + fps).c_str(), 10, 10, 12, 255, 255, 255);*/
}

void GameTest::createCar() {
	car = std::make_shared<Car>();
	car->initialize();
	car->m_body->recalculateColliders();
	Shard::Bootstrap::getInput().addListeners(car);
}

void GameTest::createAsteroid(float x, float y, float z) {
		
	auto asteroid = std::make_shared<Asteroid>();
	asteroid->initialize();
	asteroid->m_model->translate({ x, y, z });
	asteroid->m_model->scale({ 10.0f, 10.0f, 10.0f });
	asteroid->m_body->recalculateColliders();
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