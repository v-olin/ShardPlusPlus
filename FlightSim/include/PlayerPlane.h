#pragma once

#include "GameObject.h"
#include "PhysicsBody.h"
#include "InputEvent.h"
#include "InputManager.h"

class PlayerPlane : public Shard::InputListener, public Shard::GameObject, public Shard::CollisionHandler {
public:
	bool throttle_forward, throttle_back, roll_left, roll_right, pitch_up, pitch_down, yaw_left, yaw_right;
	float throttle, throttle_change;

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