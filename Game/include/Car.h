#pragma once

#include "GameObject.h"
#include "PhysicsBody.h"
#include "InputEvent.h"
#include "InputManager.h"

class PlayerPlane : public Shard::InputListener, public Shard::GameObject, public Shard::CollisionHandler {
public:
	bool throttle, backward, roll_left, roll_right, height_up, height_down;
	bool should_move;

	PlayerPlane();

	void fireBullet();
	void onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) override;
	void onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) override;
	void onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) override;

	// inherited from InputListener
	void handleEvent(Shard::InputEvent ev, Shard::EventType et);

	// inherited from GameObject
	void checkDestroyMe() override;
	void initialize() override;
	void update() override;
	void physicsUpdate() override;
	void prePhysicsUpdate() override;
	void killMe() override;

private:
};