#pragma once
#include "Game.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Model.h"

#include "Asteriod.h"
#include "Bullet.h"
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
	void createBullet();

	void update();
	int getTargetFrameRate();

	void handleEvent(Shard::InputEvent ev, Shard::EventType et);

	std::shared_ptr<Shard::Model> parent{nullptr};
	std::vector<std::shared_ptr<Asteroid>> asteroids{};
	std::vector<std::shared_ptr<Bullet>> bullets{};
	// std::shared_ptr<Spaceship> spaceship;
	std::shared_ptr<Car> car{ nullptr };

};