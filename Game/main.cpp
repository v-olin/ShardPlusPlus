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
	spaceship = std::make_unique<Spaceship>();
	Shard::Bootstrap::getInput()->addListeners(spaceship.get());
}

void GameTest::initalize() {
	Shard::Logger::log("init game");
	createShip();
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