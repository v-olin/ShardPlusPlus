#pragma once

#include "GameObject.h"
#include "PhysicsBody.h"
#include "InputEvent.h"
#include "InputManager.h"

class Asteroid : public Shard::GameObject, public Shard::InputListener, public Shard::CollisionHandler {
public:
	int torque_counter{10};

	Asteroid();

	// inherited from InputListener
	void handleEvent(Shard::InputEvent ie, Shard::EventType et);

	// inherited from GameObject
	void initialize() override;
	void physicsUpdate() override;
	void update() override;

	void onCollisionEnter(Shard::PhysicsBody* body) override;
	void onCollisionExit(Shard::PhysicsBody* body) override ;
	void onCollisionStay(Shard::PhysicsBody* body) override;


};
