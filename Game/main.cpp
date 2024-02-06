#include "main.h"

#include "Logger.h"
#include "Bootstrap.h"
#include "Spaceship.h"

#include <iostream>

#undef main
#define FAST 100000000000;
#define SLOW 50;
GameTest::GameTest() {
	// Empty constructor.
}

void GameTest::update() {
	std::string second_fps = std::to_string(Shard::Bootstrap::getSecondFPS());
	std::string fps = std::to_string(Shard::Bootstrap::getFPS());
	Shard::Display* display = Shard::Bootstrap::getDisplay();
	display->showText(("FPS: " + second_fps + " / " + fps).c_str(), 10, 10, 12, 255, 255, 255);
}

int GameTest::getTargetFrameRate() {
	return SLOW	;
}

void GameTest::createShip() {
	Shard::Logger::log("Creating spaceship");
	spaceship = std::make_unique<Spaceship>();
	Shard::Bootstrap::getInput()->addListeners(spaceship.get());
}

void GameTest::createAsteroid(float x, float y) {
	Asteroid* asteroid = new Asteroid();
	asteroid->transform_->x = x;
	asteroid->transform_->y = y;
	asteroids.push_back(asteroid);

	//Shard::Logger::log("Creating asteroid");
	//asteroid = std::make_unique<Asteroid>(x, y);
	//Shard::Bootstrap::getInput()->addListeners(asteroid.get());
}

void GameTest::initalize() {
	Shard::Logger::log("Initializing game");
	createShip();
	//for(int i = 0; i < 100; i++)
	//	createAsteroid(i%10, i%10);
	Shard::Bootstrap::getInput()->addListeners(this);
}

void GameTest::handleEvent(Shard::InputEvent ie, Shard::EventType et) {
	if (et != Shard::EventType::MouseDown)
		return;

	
	if (ie.button == SDL_BUTTON_LEFT) {
		createAsteroid(ie.x, ie.y);
	}
	else if (ie.button == SDL_BUTTON_RIGHT) {
		for (auto astr : asteroids) {
			astr->to_be_destroyed_ = true;
		}
		asteroids.clear();
	}

}


int main() {
	Shard::Logger::log("Hello from game?!?!?");

	Shard::Logger::log("Creating 'GameTest' object");
	GameTest game;
	
	Shard::Bootstrap::setRunningGame(&game);

	Shard::Logger::log("Runnning Bootstrap::Main");
	Shard::Bootstrap::Main({});

	Shard::Logger::log("Exited out of Bootstrap::Main. Exiting program.");
	return 0;
}