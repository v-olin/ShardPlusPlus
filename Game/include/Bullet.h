#pragma once

#include "GameObject.h"
#include "Spaceship.h"
#include "PhysicsBody.h"
#include "CollisionHandler.h"

class Bullet : public Shard::GameObject, public Shard::CollisionHandler {
public:
	Bullet();
	void setupBullet(Spaceship* origin_spaceship, float x, float y);
	void initialize() override;
	void physicsUpdate() override;
	void prePhysicsUpdate() override;
	void update() override;

	void onCollisionEnter(Shard::PhysicsBody* body) override;
	void onCollisionExit(Shard::PhysicsBody* body) override;
	void onCollisionStay(Shard::PhysicsBody* body) override;


	void killMe() override;

private:
	Spaceship* origin;
};
