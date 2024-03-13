#pragma once
#include "Game.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Model.h"

#include "Bullet.h"
#include "PlayerPlane.h"
#include "AIPlane.h"
#include "AttitudeIndicator.h"
#include "HeadingIndicator.h"

#include <vector>
#include <memory>

class GameTest : public Shard::Game, public Shard::InputListener, public std::enable_shared_from_this<GameTest> {
public:
	GameTest() = default;

	void initalize();
	void createPlayerPlane();
	void createBullet();
	void createAIPlane(float x, float y, float z);

	void update();
	int getTargetFrameRate();

	void handleEvent(Shard::InputEvent ev, Shard::EventType et);

	//parent can be used if we want to create many enemy planes
	std::shared_ptr<Shard::Model> AI_parent{nullptr};
	std::shared_ptr<Shard::Model> bullet_parent{nullptr};
	std::vector<std::shared_ptr<AIPlane>> aiPlanes{};
	std::vector<std::shared_ptr<Bullet>> bullets{};
	std::shared_ptr<PlayerPlane> playerPlane{ nullptr };
	std::shared_ptr<AttitudeIndicator> planeAI{ nullptr };
	std::shared_ptr<HeadingIndicator> planeHI{ nullptr };

	std::shared_ptr <Shard::GameObject> lockedTarget;



	bool usePathTracing{ false };
	int pathTracingSampling{ 4 };
	bool drawColliders{ false };
	int target_framerate{ 144 };
	glm::vec3 fp_offset{7, 2.8f, 0};


};