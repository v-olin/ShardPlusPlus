#include "Bullet.h"
#include "Bootstrap.h"
#include "Logger.h"

#include <cstdlib>

Bullet::Bullet() : GameObject(), origin(nullptr) {
	initialize();
}

void Bullet::setupBullet(Spaceship* origin_spaceship, float x, float y) {
	setPhysicsEnabled();
	transform_->x = x;
	transform_->y = y;
	transform_->w = 10;
	transform_->h = 10;

	this->origin = origin_spaceship;

	body_->mass = 100.0f;
	body_->max_force = 50.0f;
	body_->pass_through = true;

	body_->addRectCollider((int)x, (int)y, 10, 10);

	GameObject::addTag("Bullet");
}

void Bullet::initialize() {
	transient_ = true;
}

void Bullet::physicsUpdate() {
	body_->addForce(transform_->forward, 100.0f);
}

void Bullet::update() {
	unsigned char r = rand() % 256, g = rand() % 256;
	SDL_Color color = { r, g, 0, 255 };

	Shard::Display* disp = Shard::Bootstrap::getDisplay();

	disp->drawLine(
		transform_->x,
		transform_->y,
		transform_->x + 10,
		transform_->y + 10,
		color
	);

	disp->drawLine(
		transform_->x + 10,
		transform_->y,
		transform_->x,
		transform_->y + 10,
		color
	);
}

void Bullet::onCollisionEnter(Shard::PhysicsBody* body) {

	Shard::Logger::log("INSIDE ONCOLLISIONENTER BULLETtt tttttttt@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", Shard::LoggerLevel::LOG_LEVEL_ALL);

	if (body->parent->hasTag("Spaceship") == false) {
		Shard::Logger::log("Boom! (bullet ded)");
		to_be_destroyed_ = true;
	}
}
void Bullet::onCollisionExit(Shard::PhysicsBody* body) {}
void Bullet::onCollisionStay(Shard::PhysicsBody* body) {}
void Bullet::killMe() {}
void Bullet::prePhysicsUpdate(){}
