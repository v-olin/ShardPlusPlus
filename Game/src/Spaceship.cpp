#include "Spaceship.h"
#include "Logger.h"
#include "Bullet.h"
#include "Bootstrap.h"
#include "Logger.h"
#include "GameObjectManager.h"

#include <SDL.h>

Spaceship::Spaceship() : GameObject() {
	//initialize();
	//Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
}

void Spaceship::fireBullet() {
    Bullet* b = new Bullet();
    Shard::Logger::log("FIRE");

    // THIS SHOULD NOT BE DONE HERE,
    // https://tenor.com/view/oh-no-oh-no-anyway-gif-18887547 
    b->setupBullet(body_->trans->centre.x, body_->trans->centre.y);
    b->body_->trans->rotate(body_->trans->rotz);
    Shard::Bootstrap::getSound().playSound("fire.wav");
}

void Spaceship::handleEvent(Shard::InputEvent ev, Shard::EventType et) {
    //Shard::Logger::log("handleEvent in ship");
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
    body_->trans->x = 500.f;
    body_->trans->y = 300.f;
    auto path = Shard::Bootstrap::getAssetManager().getAssetPath("spaceship.png");
    body_->trans->sprite_path = path;

    // if you move this stuff above transform_ init ^ then colliders will not be drawn
    // why? ... you figure it out!
	up = false;
	down = false;
	body_->mass = 1.f;
	body_->max_force = 1000.f;
	body_->angular_drag = 0.01f;
	body_->drag = 0.1f;
	body_->stop_on_collision = true;
	body_->reflect_on_collision = true;
	body_->impart_force = false;
	body_->is_kinematic = false;
    body_->pass_through = false;

    body_->addRectCollider();
    //body_->addCircleCollider();
    
    GameObject::addTag("Spaceship");
}

void Spaceship::update() {
 
    Shard::Bootstrap::getDisplay()->addToDraw(shared_from_this());
}

void Spaceship::physicsUpdate() {
    //we need to change drag and the force/torque we add depending on the framerate
    auto frames = Shard::Bootstrap::getFPS();
    //50 frames: angular_drag = .01, drag = .1, torque = .3, force = .25/-.05
    body_->angular_drag = frames / 3000;
    body_->drag = frames / 500;

    if (turn_left)
        body_->addTorque(-0.3f);
    if (turn_right)
        body_->addTorque(0.3f);
    if (up)
        body_->addForce(body_->trans->forward, 0.5f);
    if (down)
        body_->addForce(body_->trans->forward, -0.2f);
}

void Spaceship::prePhysicsUpdate() {
    // TODO: Should this be empty?
}

void Spaceship::killMe() {
    // TODO: Clean up!!!
}

void Spaceship::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {
    Shard::Logger::log("INDIDE ONCOLLISIONENTER SPACESHIP");
    if (!body->parent->hasTag("Bullet"))
        body_->debug_color_ = { 255, 0, 0, 255 };
    // TODO: Lower HP?
}

void Spaceship::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {
    // TODO: Not sure...
}

void Spaceship::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {
    // TODO: Not sure...
    if (!body->parent->hasTag("Bullet"))
        body_->debug_color_ = { 0, 0, 255, 255 };
}
