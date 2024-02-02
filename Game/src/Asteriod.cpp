#include "Asteriod.h"

#include "Bootstrap.h"
#include "Logger.h"

void handleAsteroidInput(Shard::InputEvent ev, Shard::EventType et) {

}

Asteroid::Asteroid() : GameObject() {
	initialize();
}

void Asteroid::handleEvent(Shard::InputEvent ie, Shard::EventType et) {
    if (et == Shard::EventType::MouseDown && ie.button == SDL_BUTTON_MIDDLE) {
        if ((body_->checkCollisions(glm::vec2{ ie.x, ie.y })).has_value()) {
            torque_counter += 10;
        }
    }
}

void Asteroid::initialize()
{
    setPhysicsEnabled();
	transform_->x = 300.0f;
	transform_->y = 300.0f;
	auto path = Shard::Bootstrap::getAssetManager()->getAssetPath("asteroid.png");
	transform_->sprite_path = path;

    body_->max_torque = 50;
    body_->max_force = 100;
    body_->mass = 1;
    body_->uses_gravity = true;
    body_->stop_on_collision = false;
    body_->reflect_on_collision = true;
    body_->is_kinematic = false;
    
    // TODO: Add randomness to initial force
    //body_->addForce(transform_->right, 2.0f);

    body_->addRectCollider();

    Shard::Bootstrap::getInput()->addListeners(this);

    GameObject::addTag("Asteroid");

}
void Asteroid::update(){
    Shard::Bootstrap::getDisplay()->addToDraw(this);
}

void Asteroid::physicsUpdate() {
    for (int i = 0; i < torque_counter; i++)
        body_->addTorque(0.1f);

    if (torque_counter > 0)
        torque_counter -= 1;
}

void Asteroid::onCollisionEnter(Shard::PhysicsBody* body) {

    Shard::Logger::log("INSIDE ONCOLLISIONENTER ASTEROOOOOOOOID@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", Shard::LoggerLevel::LOG_LEVEL_ALL);

    if (body->parent->hasTag("Bullet")) {
        to_be_destroyed_ = true;
		Shard::Logger::log("Boom!");
    }

    Shard::Logger::log("Bang!");
}

void Asteroid::onCollisionExit(Shard::PhysicsBody* body) {
    Shard::Logger::log("Anti bang!");
}

void Asteroid::onCollisionStay(Shard::PhysicsBody* body) { }
void Asteroid::killMe(){
}
void Asteroid::prePhysicsUpdate(){}
