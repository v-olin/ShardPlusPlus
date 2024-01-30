#include "main.h"

#include "Logger.h"
#include "Bootstrap.h"
#include "Spaceship.h"

#include <iostream>
#include <memory>

#undef main

std::unique_ptr<Spaceship> ship_pointer;

void GameTest::update() {
	std::string second_fps = std::to_string(Shard::Bootstrap::getSecondFPS());
	std::string fps = std::to_string(Shard::Bootstrap::getFPS());
	Shard::Display* display = Shard::Bootstrap::getDisplay();
	display->showText(("FPS: " + second_fps + " / " + fps).c_str(), 10, 10, 12, 255, 255, 255);
	ship_pointer->update();
}

int GameTest::getTargetFrameRate() {
	return 10;
}


//void handleSpaceShipInput(Shard::InputEvent ev, Shard::EventType et) {
//    ship_pointer->handleEvent(ev, et);
//}

void GameTest::createShip() {
	//THIS CRASHES IT:
	//ship_pointer = new Spaceship();

	ship_pointer = std::make_unique<Spaceship>();
	Shard::Bootstrap::getInput()->addListeners(ship_pointer.get());
	//Random rand = new Random();
	int offsetx = 0, offsety = 0;

	//Shard::GameObject asteroid;


	//background = new Shard::GameObject();
	//background->transform_.sprite_path = Shard::AssetManager::getInstance().getAssetPath("background2.jpg");
	//background->transform_.x = 0;
	//background->transform_.y = 0;
}

void GameTest::initalize() {
	Shard::Logger::log("init game");
	createShip();
}

int main() {
	Shard::Logger::log("Hello from game?!?!?");

	Shard::Logger::log("Runnning Bootstrap::Main");
	GameTest game;
	Shard::Bootstrap::setRunningGame(&game);
	Shard::Bootstrap::Main({});

	Shard::Logger::log("Done?!?");

	return 0;
}