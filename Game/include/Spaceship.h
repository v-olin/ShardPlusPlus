#pragma once

#include "GameObject.h"
#include "PhysicsBody.h"
#include "InputEvent.h"
#include "InputManager.h"

class Spaceship : public Shard::InputListener, public Shard::GameObject {
public:
	bool up, down, turn_left, turn_right;

	Spaceship();
	Spaceship(const Spaceship* src) : Shard::GameObject(this) {
		this->up = src->up;
		this->down = src->down;
		this->turn_left = src->turn_left;
		this->turn_right = src->turn_right;
	}

	//Spaceship(const Spaceship* src) {
	//	this->body_ = src->body_;
	//	this->transform_ = src->transform_;
	//	this->transient_ = src->transient_;
	//	this->to_be_destroyed_ = src->to_be_destroyed_;
	//	this->visible_ = src->visible_;
	//	this->tags = src->tags;
	//	this->up = src->up;
	//	this->down = src->down;
	//	this->turn_left = src->turn_left;
	//	this->turn_right = src->turn_right;
	//}

	void fireBullet();
	void onCollisionEnter(Shard::PhysicsBody body);
	void onCollisionExit(Shard::PhysicsBody body);
	void onCollisionStay(Shard::PhysicsBody body);

	// inherited from InputListener
	void handleEvent(Shard::InputEvent ev, Shard::EventType et);

	// inherited from GameObject
	void initialize() override;
	void update() override;
	void physicsUpdate() override;
	void prePhysicsUpdate() override;
	void killMe() override;

private:
};