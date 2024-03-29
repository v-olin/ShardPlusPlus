#include "PhysicsManager.h"
#include "GameObject.h"
#include "Logger.h"
#include "Bootstrap.h"
#include "Car.h"
#include "Asteriod.h"
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
	}
	playerPlane->should_move = !is_rmb_down;// cameraStatus != Shard::CameraView::FREE;
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
				Shard::PathTracer::restart();
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


	/*if(cam_firstPerson == 1)
		sm.camera.status = Shard::CameraView::FIRST_PERSON;
	if(cam_thirdPerson == 1)
		sm.camera.status = Shard::CameraView::THIRD_PERSON;
	if(cam_free == 1)
		sm.camera.status = Shard::CameraView::FREE;
	if(cam_lock == 1)
		sm.camera.status = Shard::CameraView::LOCK;
	*/

	sm.camera.status = (Shard::CameraView)cameraStatus;


	if (c_forward || c_backward || c_left || c_right || c_up || c_down)
		Shard::PathTracer::restart();

	sm.camera.updateCameraToPlayer();

	Shard::Bootstrap::setUsePathTracing(usePathTracing);
	Shard::Bootstrap::setEnvironmentVariable("physics_debug", drawColliders ? "1" : "0");



	static int oldSampling = 0;
	if (oldSampling != pathTracingSampling){
		Shard::PathTracer::settings.subsampling = pathTracingSampling; 
		Shard::PathTracer::restart();
		oldSampling = pathTracingSampling;
	}




	/*Shard::Display* display = Shard::Bootstrap::getDisplay();
	display->showText(("FPS: " + second_fps + " / " + fps).c_str(), 10, 10, 12, 255, 255, 255);*/
}

void GameTest::createPlayerPlane() {
	playerPlane = std::make_shared<PlayerPlane>();
	playerPlane->initialize();
	playerPlane->m_body->recalculateColliders();
	Shard::Bootstrap::setPlane(playerPlane->m_model);
	Shard::Bootstrap::getInput().addListeners(playerPlane);
	static auto &sm = Shard::SceneManager::getInstance();
	sm.camera.setPlayerGameObj(playerPlane);
	sm.camera.setFirstPersonOffset(glm::vec3(-9, 8, 0));
	sm.camera.setThirdPersonOffset(glm::vec3(50, 25, 0), glm::vec3(0, 15, 0));
}


void GameTest::createBullet() {
	auto bullet = std::make_shared<Bullet>();
	bullet->initialize();
	bullet->m_model->scale({10, 10, 10});
	bullet->m_model->translate(playerPlane->m_model->position());
	bullet->m_model->m_rotMatrix = playerPlane->m_model->getRotationMatrix();
	bullet->m_body->recalculateColliders();
	bullets.push_back(bullet);
}
float randf() {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}
void GameTest::createAsteroid(float x, float y, float z) {
	auto asteroid = std::make_shared<Asteroid>();
	asteroid->m_model = std::make_shared<Shard::Model>(parent);
	asteroid->initialize();
	asteroid->m_model->translate({ x, y, z });
	float scale = ((rand() % 50) +1) / 100.f;
	asteroid->m_model->scale({ scale, scale, scale });
	asteroid->m_body->recalculateColliders();
	int posX = (rand() % 10 > 5) ? 1 : -1;
	int posY = (rand() % 10 > 5) ? 1 : -1;
	int posZ = (rand() % 10 > 5) ? 1 : -1;
	auto dir = normalize(glm::vec3(rand()*posX, rand()*posY, rand()*posZ));
	asteroid->m_body->addForce(dir, 0.2f);
	asteroid->m_body->addTorque({ 1*randf(), 1*randf(), 1*randf()});
    Shard::Bootstrap::getInput().addListeners(asteroid);

	asteroids.push_back(asteroid);
}


void GameTest::initalize() {
	Shard::Logger::log("Initializing game");
	createPlayerPlane();
	int max = 100;
	parent = std::make_shared<Shard::Model>("models/asteroid_fixed.obj");
	for (int i = 0; i < 1; i++) {
		auto pos = glm::vec3(rand() % max, rand() % max, rand() % max) - glm::vec3(max/2, 0, max/2);
		createAsteroid(pos.x, pos.y, pos.z);
	}

	//createFlatPlane(0, -12, 0);
	Shard::Bootstrap::getInput().addListeners(shared_from_this());
	static auto &sm = Shard::SceneManager::getInstance();
	sm.camera.movementSpeed = 100;

	drawColliders = Shard::Bootstrap::getEnvironmentVariable("physics_debug") == "1";


	//GUI
	Shard::Bootstrap::gui->addRadioSelector({ "View",  
		&cameraStatus,
		{
		std::make_tuple("First person", Shard::CameraView::FIRST_PERSON),
		std::make_tuple("Third person", Shard::CameraView::THIRD_PERSON),
		std::make_tuple("Lock camera on Obj", Shard::CameraView::LOCK),
		std::make_tuple("Free Cam", Shard::CameraView::FREE),
		} });

	Shard::Bootstrap::gui->addCheckBox("Use Path Tracing", &usePathTracing);
	Shard::Bootstrap::gui->addCheckBox("Draw colliders", &drawColliders);

	Shard::Bootstrap::gui->addIntSlider("Path tracer sampling", &pathTracingSampling, 1, 16);


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