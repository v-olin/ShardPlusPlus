//#include "Bullet.h"
//#include "Bootstrap.h"
//#include "Logger.h"
//
//#include <cstdlib>
//
//Bullet::Bullet() : GameObject() {
//	initialize();
//}
//
//void Bullet::setupBullet(float x, float y) {
//	setPhysicsEnabled();
//	body_->trans->x = x;
//	body_->trans->y = y;
//	body_->trans->w = 10;
//	body_->trans->h = 10;
//
//	body_->mass = 100.0f;
//	body_->max_force = 50.0f;
//	body_->pass_through = true;
//
//	body_->addRectCollider((int)x, (int)y, 10, 10);
//
//	GameObject::addTag("Bullet");
//}
//
//void Bullet::initialize() {
//	transient_ = true;
//}
//
//void Bullet::physicsUpdate() {
//	body_->addForce(body_->trans->forward, 100.0f);
//}
//
//void Bullet::update() {
//	unsigned char r = rand() % 256, g = rand() % 256;
//	SDL_Color color = { r, g, 0, 255 };
//
//	Shard::Display* disp = Shard::Bootstrap::getDisplay();
//
//	disp->drawLine(
//		body_->trans->x,
//		body_->trans->y,
//		body_->trans->x + 10,
//		body_->trans->y + 10,
//		color
//	);
//
//	disp->drawLine(
//		body_->trans->x + 10,
//		body_->trans->y,
//		body_->trans->x,
//		body_->trans->y + 10,
//		color
//	);
//}
//
//void Bullet::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {
//
//	Shard::Logger::log("INSIDE ONCOLLISIONENTER BULLETtt tttttttt@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", Shard::LoggerLevel::LOG_LEVEL_ALL);
//
//	if (body->parent->hasTag("Car") == false) {
//		Shard::Logger::log("Boom! (bullet ded)");
//		to_be_destroyed_ = true;
//	}
//}
//void Bullet::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {}
//void Bullet::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {}
//void Bullet::killMe() {}
//void Bullet::prePhysicsUpdate(){}
