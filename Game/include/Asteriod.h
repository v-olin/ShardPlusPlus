#pragma once

#include "GameObject.h"
#include "PhysicsBody.h"
#include "InputEvent.h"
#include "InputManager.h"

#include <memory>

class Asteroid
	: public Shard::GameObject
	, public Shard::InputListener
	, public Shard::CollisionHandler
	// , public std::enable_shared_from_this<Asteroid> 
	{
public:
	int torque_counter{10};

	Asteroid();

	// inherited from InputListener
	void handleEvent(Shard::InputEvent ie, Shard::EventType et);

	// inherited from GameObject
	void initialize() override;
	void physicsUpdate() override;
    void prePhysicsUpdate() override;
	void update() override;
	void killMe() override;

	void onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) override;
	void onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) override ;
	void onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) override;


};
