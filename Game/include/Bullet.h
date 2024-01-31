#pragma once

#include "GameObject.h"
#include "Spaceship.h"
#include "PhysicsBody.h"
#include "CollisionHandler.h"

class Bullet : public Shard::GameObject {
public:
	Bullet();
	void setupBullet(Spaceship* origin_spaceship, float x, float y);
	void initialize();
	void physicsUpdate();
	void update();

	void onCollisionEnter(Shard::PhysicsBody body);
	void onCollisionExit(Shard::PhysicsBody body);
	void onCollisionStay(Shard::PhysicsBody body);

private:
	Spaceship* origin;
};
