//#include "main.h"
//
//#include "Logger.h"
//#include "Bootstrap.h"
//#include "Spaceship.h"
//#include "GameObjectManager.h"
//#include <iostream>
//
//#undef main
//#define FAST 100000000000;
//#define SLOW 50;
//
//GameTest::GameTest() {
//	// Empty constructor.
//}
//
//void GameTest::update() {
//	std::string second_fps = std::to_string(Shard::Bootstrap::getSecondFPS());
//	std::string fps = std::to_string(Shard::Bootstrap::getFPS());
//	Shard::Display* display = Shard::Bootstrap::getDisplay();
//	display->showText(("FPS: " + second_fps + " / " + fps).c_str(), 10, 10, 12, 255, 255, 255);
//}
//
//int GameTest::getTargetFrameRate() {
//	return SLOW	;
//}
//
//void GameTest::createShip() {
//	Shard::Logger::log("Creating spaceship");
//	// shared_ptr<A> a( new A );
//	spaceship = std::make_unique<Spaceship>();
//	spaceship->initialize();
//	Shard::GameObjectManager::getInstance().addGameObject(spaceship->shared_from_this());
//	Shard::Bootstrap::getInput().addListeners(spaceship);
//}
//
//void GameTest::createAsteroid(float x, float y) {
//	auto asteroid = std::make_shared<Asteroid>();
//	asteroid->initialize();
//	Shard::GameObjectManager::getInstance().addGameObject(asteroid->shared_from_this());
//	//TODO: this will have to be fixed
//	//asteroid->body_->trans->x = x;
//	//asteroid->body_->trans->y = y;
//	asteroids.push_back(asteroid);
//}
//
//void GameTest::initalize() {
//	Shard::Logger::log("Initializing game");
//	createShip();
//	//for(int i = 0; i < 100; i++)
//	//	createAsteroid(i%10, i%10);
//	Shard::Bootstrap::getInput().addListeners(shared_from_this());
//}
//
//void GameTest::handleEvent(Shard::InputEvent ie, Shard::EventType et) {
//	if (et != Shard::EventType::MouseDown)
//		return;
//
//	
//	if (ie.button == SDL_BUTTON_LEFT) {
//		for(int i = 0; i < 1; i++)
//			createAsteroid(ie.x, ie.y);
//	}
//	else if (ie.button == SDL_BUTTON_RIGHT) {
//		for (auto astr : asteroids) {
//			astr->to_be_destroyed_ = true;
//		}
//		asteroids.clear();
//	}
//
//}
//
//
//int main() {
//	Shard::Logger::log("Hello from game?!?!?");
//
//	Shard::Logger::log("Creating 'GameTest' object");
//
//	auto game = std::make_shared<GameTest>();
//	Shard::Bootstrap::setRunningGame(game);
//
//	Shard::Logger::log("Runnning Bootstrap::Main");
//	Shard::Bootstrap::Main({});
//
//	Shard::Logger::log("Exited out of Bootstrap::Main. Exiting program.");
//	return 0;
//}

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
    
	void onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) override {};
    void onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) override {};
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
	//manager.addPhysicsObject(gob1->body_->shared_from_this());
	//manager.addPhysicsObject(gob2->body_->shared_from_this());

	// Tick
	manager.update();

	// Check collisions
	if (manager.collision_last_frame == 0)
		passed = 1;

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
	//manager.addPhysicsObject(gob1->body_->shared_from_this());
	//manager.addPhysicsObject(gob2->body_->shared_from_this());

	// Tick
	manager.update();

	// Check collisions
	if (manager.collision_last_frame > 0)
		passed = 1;

	// Result?
	std::cout << "Result test 2: " << std::to_string(passed == 1) << "\n";
	return passed;
}

int main() {

	std::cout << "Testing shit... 0 means it failed and 1 means it passed.\n\n";
	
	int t1 = Test1(); // Should fail, no collision.
	int t2 = Test2(); // Should pass, expecting collision.
	
	std::cout << "\nhaha, you suck! your tests failed lol (safe assumption being made here)\n";

	// Everything will always be fine.
	return 0;

}