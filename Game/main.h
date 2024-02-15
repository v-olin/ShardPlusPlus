#pragma once
#include "Game.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "GameObject.h"

#include "Asteriod.h"
#include "Car.h"

#include <vector>
#include <memory>

class GameTest : public Shard::Game, public Shard::InputListener, public std::enable_shared_from_this<GameTest> {
public:
	GameTest() = default;

	void initalize();
	// void createShip();
	void createCar();
	void createAsteroid(float x, float y, float z);

	void update();
	int getTargetFrameRate();

	void handleEvent(Shard::InputEvent ev, Shard::EventType et);

	std::vector<std::shared_ptr<Asteroid>> asteroids{};
	// std::shared_ptr<Spaceship> spaceship;
	std::shared_ptr<Car> car{ nullptr };

};