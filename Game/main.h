#pragma once
#include "Game.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Spaceship.h"

#include <vector>
#include <memory>

class GameTest : public Shard::Game {

	//class GameTest {
public:
	GameTest();
	void initalize();
	void createShip();

	void update();
	int getTargetFrameRate();

	//void handleInput(Shard::InputEvent ev, Shard::EventType et);

	std::unique_ptr<Spaceship> spaceship{ nullptr };
};
