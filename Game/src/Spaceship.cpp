#include "Spaceship.h"
#include "Logger.h"
#include "Bootstrap.h"
#include "Logger.h"

#include <SDL.h>

Spaceship::Spaceship() : GameObject() {
	initialize();
}

void Spaceship::fireBullet() {}

void Spaceship::handleEvent(Shard::InputEvent ev, Shard::EventType et) {
    Shard::Logger::log("handleEvent in ship");
    if (et == Shard::EventType::KeyDown)
    {
        if (ev.key == (int)SDL_SCANCODE_W)
            up = true;

        if (ev.key == (int)SDL_SCANCODE_S)
            down = true;

        if (ev.key == (int)SDL_SCANCODE_D)
            turn_right = true;

        if (ev.key == (int)SDL_SCANCODE_A)
            turn_left = true;

    }
    else if (et == Shard::EventType::KeyUp)
    {
        if (ev.key == (int)SDL_SCANCODE_W)
            up = false;

        if (ev.key == (int)SDL_SCANCODE_S)
            down = false;

        if (ev.key == (int)SDL_SCANCODE_D)
            turn_right = false;

        if (ev.key == (int)SDL_SCANCODE_A)
            turn_left = false;
    }

    if (et == Shard::KeyUp )
    {
        if (ev.key == (int)SDL_SCANCODE_SPACE)
            fireBullet();
    }
}

void Spaceship::initialize() {

    setPhysicsEnabled(); // sets body_ to a new PhysicBody(this ) and populates transform_
    transform_->x = 500.f;
    transform_->y = 500.f;
    auto path = Shard::Bootstrap::getAssetManager()->getAssetPath("spaceship.png");
    transform_->sprite_path = path;

    // if you move this stuff above transform_ init ^ then colliders will not be drawn
    // why? ... you figure it out!
	up = false;
	down = false;
	body_->mass = 1.f;
	body_->max_force = 10.f;
	body_->angular_drag = 0.01f;
	body_->drag = 0.f;
	body_->stop_on_collision = false;
	body_->reflect_on_collision = false;
	body_->impart_force = false;
	body_->is_kinematic = false;

    body_->addRectCollider();
    //body_->addCircleCollider();
    
    GameObject::addTag("Spaceship");
}

void Spaceship::update() {
    // TODO: ???
    //0x00000295ee25f3a0
    //0x00000295ee25f5e0
    Shard::Bootstrap::getDisplay()->addToDraw(this);
}

void Spaceship::physicsUpdate() {
    if (turn_left)
        body_->addTorque(-0.3f);
    if (turn_right)
        body_->addTorque(0.3f);
    if (up)
        body_->addForce(body_->trans.forward, 0.25f);
    if (down)
        body_->addForce(body_->trans.forward, -0.05f);
}

void Spaceship::prePhysicsUpdate() {
    // TODO: Should this be empty?
}

void Spaceship::killMe() {
    // TODO: Clean up!!!
}

void Spaceship::onCollisionEnter(Shard::PhysicsBody body) {
    // TODO: Lower HP?
}

void Spaceship::onCollisionExit(Shard::PhysicsBody body) {
    // TODO: Not sure...
}

void Spaceship::onCollisionStay(Shard::PhysicsBody body) {
    // TODO: Not sure...
}
