#include "Asteriod.h"

#include "Bootstrap.h"

void handleAsteroidInput(Shard::InputEvent ev, Shard::EventType et) {

}

Asteroid::Asteroid() {
	initialize();
}


void Asteroid::initialize()
{
    transform_.sprite_path = Shard::Bootstrap::getAssetManager()->getAssetPath("asteriod.png");

    setPhysicsEnabled();
    body_->max_torque = 100;
    body_->mass = 1;
    body_->max_force = 100;
    body_->uses_gravity = true;
    body_->stop_on_collision = false;
    body_->reflect_on_collision = true;
    //            MyBody.Kinematic = true;
    body_->addForce(transform_.right, 20.5f);


    body_->addRectCollider();

    //Shard::Bootstrap::getInput()->addListeners(&handleAsteroidInput);

    addTag("Asteroid");

}
void Asteroid::update(){}
void Asteroid::onCollisionEnter(Shard::PhysicsBody body) {}
void Asteroid::onCollisionExit(Shard::PhysicsBody body) {}
void Asteroid::onCollisionStay(Shard::PhysicsBody body) {}
void Asteroid::physicsUpdate() {}

