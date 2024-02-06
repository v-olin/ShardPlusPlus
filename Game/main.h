#pragma once
#include "Game.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "GameObject.h"

#include "Asteriod.h"
#include "Spaceship.h"

#include <vector>
#include <memory>

class GameTest : public Shard::Game, public Shard::InputListener {
public:
	GameTest();

	void initalize();
	void createShip();
	void createAsteroid(float x, float y);

	void update();
	int getTargetFrameRate();

	void handleEvent(Shard::InputEvent ev, Shard::EventType et);

	std::vector<Asteroid*> asteroids{};
	//std::vedctor<Asteroid*> asteroids{ nullptr };
	//std::unique_ptr<Asteroid> asteroid{ nullptr };
	std::unique_ptr<Spaceship> spaceship{ nullptr };
};
