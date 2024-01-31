#include "Spaceship.h"
#include "Logger.h"
#include "Bootstrap.h"
#include <SDL.h>
#include "Logger.h"

Spaceship::Spaceship() : GameObject() {
    //GameObject::initialize();
	initialize();
}

void Spaceship::fireBullet() {}

void Spaceship::handleEvent(Shard::InputEvent ev, Shard::EventType et) {
    Shard::Logger::log("handleEvent in ship");
    if (et == Shard::EventType::KeyDown)
    {
        if (ev.key == (int)SDL_SCANCODE_W)
        {
            up = true;
        }

        if (ev.key == (int)SDL_SCANCODE_S)
        {
            down = true;
        }

        if (ev.key == (int)SDL_SCANCODE_D)
        {
            turn_right = true;
        }

        if (ev.key == (int)SDL_SCANCODE_A)
        {
            turn_left = true;
        }

    }
    else if (et == Shard::EventType::KeyUp)
    {
        if (ev.key == (int)SDL_SCANCODE_W)
        {
            up = false;
        }

        if (ev.key == (int)SDL_SCANCODE_S)
        {
            down = false;
        }

        if (ev.key == (int)SDL_SCANCODE_D)
        {
            turn_right = false;
        }

        if (ev.key == (int)SDL_SCANCODE_A)
        {
            turn_left = false;
        }
    }

    if (et == Shard::KeyUp )
    {
        if (ev.key == (int)SDL_SCANCODE_SPACE)
        {
            fireBullet();
        }
    }
}

void Spaceship::initialize() {
    setPhysicsEnabled();
	transform_->x = 500.f;
	transform_->y = 500.f;
    auto path = Shard::Bootstrap::getAssetManager()->getAssetPath("spaceship.png");
    transform_->sprite_path = path;

	up = false;
	down = false;

    //body_ = new Shard::PhysicsBody();

	body_->mass = 1.f;
	body_->max_force = 10.f;
	body_->angular_drag = 0.01f;
	body_->drag = 0.f;
	body_->stop_on_collision = false;
	body_->reflect_on_collision = false;
	body_->impart_force = false;
	body_->is_kinematic = false;

    // i just want to drive my fucking spaceship 
    //(commented since it needs pgysics body)
	body_->addRectCollider();

    //const char* a = "Spacheship";
    //GameObject::addTag(a);
    //tags.push_back("asdkaskd");
    
    auto t = tags.size();
    GameObject::addTag("Spaceship");
}

void Spaceship::update() {
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
        body_->addForce(transform_->forward, 0.5f);
    if (down)
        body_->addForce(transform_->forward, -0.2);
}

void Spaceship::prePhysicsUpdate() {

}

void Spaceship::killMe() {

}
void Spaceship::onCollisionEnter(Shard::PhysicsBody body) {}
void Spaceship::onCollisionExit(Shard::PhysicsBody body) {}
void Spaceship::onCollisionStay(Shard::PhysicsBody body) {}
