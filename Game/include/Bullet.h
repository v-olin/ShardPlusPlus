//#pragma once
//
//#include "GameObject.h"
//#include "Car.h"
//#include "PhysicsBody.h"
//#include "CollisionHandler.h"
//
//class Bullet : public Shard::GameObject, public Shard::CollisionHandler {
//public:
//	Bullet();
//	void setupBullet(float x, float y);
//	void initialize() override;
//	void physicsUpdate() override;
//	void prePhysicsUpdate() override;
//	void update() override;
//
//	void onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) override;
//	void onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) override;
//	void onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) override;
//
//	void killMe() override;
//
//private:
//};
#pragma once

#include "GameObject.h"
#include "PhysicsBody.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "Logger.h"

#include <memory>

class Bullet
	: public Shard::GameObject
	, public Shard::CollisionHandler
	{
public:

	Bullet();

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


};

