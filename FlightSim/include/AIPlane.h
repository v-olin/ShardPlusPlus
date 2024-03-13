#pragma once

#include "GameObject.h"
#include "PhysicsBody.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "Logger.h"

#include <memory>

class AIPlane
	: public Shard::GameObject
	, public Shard::InputListener
	, public Shard::CollisionHandler
{
public:

	AIPlane();
	void generateTargetPonts();

	// inherited from InputListener
	void handleEvent(Shard::InputEvent ie, Shard::EventType et);

	// inherited from GameObject
	void checkDestroyMe() override;
	void initialize() override;
	void physicsUpdate() override;
	void prePhysicsUpdate() override;
	void update() override;
	void killMe() override;

	void onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) override;
	void onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) override;
	void onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) override;

	std::vector<glm::vec3> target_points;	
	long long m_hunt_start_time;
	long long target_hunt_time;
	int m_hunt_index;




};
