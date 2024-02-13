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
