/*

#pragma once

#include "GameObject.h"
#include "PhysicsBody.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "Logger.h"

#include <memory>

class Building
	: public Shard::GameObject
	, public Shard::CollisionHandler
	{
public:

	Building();

	// inherited from GameObject
	void checkDestroyMe() override;
	void initialize() override;
	void physicsUpdate() override;
    void prePhysicsUpdate() override;
	void update() override;
	void killMe() override;

	void onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) override;
	void onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) override ;
	void onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) override;
	private:
};

*/
