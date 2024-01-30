#pragma once
#include "Game.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Spaceship.h"

#include <vector>

class GameTest : public Shard::Game {

	//class GameTest {
public:
	GameTest() {};
	void update();
	int getTargetFrameRate();

	void initalize();

	void createShip();

	//void handleInput(Shard::InputEvent ev, Shard::EventType et);


	Shard::GameObject* background;
	//std::vector<Shard::GameObject> asteriods;


};
