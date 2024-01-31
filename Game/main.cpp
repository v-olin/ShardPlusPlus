#include "main.h"

#include "Logger.h"
#include "Bootstrap.h"
#include "Spaceship.h"

#include <iostream>

#undef main

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
	return 50;
}

void GameTest::createShip() {
	Shard::Logger::log("Creating spaceship");
	spaceship = std::make_unique<Spaceship>();
	Shard::Bootstrap::getInput()->addListeners(spaceship.get());
}

void GameTest::createAsteroid() {
	Shard::Logger::log("Creating asteroid");
	asteroid = std::make_unique<Asteroid>();
	Shard::Bootstrap::getInput()->addListeners(asteroid.get());
}

void GameTest::initalize() {
	Shard::Logger::log("Initializing game");
	createShip();
	// TODO: Create more than one.
	createAsteroid();
}

void GameTest::handleEvent(Shard::InputEvent ie, Shard::EventType et) {
	if (et != Shard::EventType::MouseDown)
		Shard::Logger::log("Pressing button " + ie.button);
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