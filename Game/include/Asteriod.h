#pragma once

#include "GameObject.h"
#include "PhysicsBody.h"

class Asteroid : public Shard::GameObject {
public:
	int toque_center = 0;



	Asteroid();

	void initialize() override;
	void physicsUpdate() override;
	void update() override;

	void onCollisionEnter(Shard::PhysicsBody body);
	void onCollisionExit(Shard::PhysicsBody body);
	void onCollisionStay(Shard::PhysicsBody body);


};
