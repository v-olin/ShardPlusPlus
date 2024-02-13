#include "Car.h"
#include "Logger.h"
#include "Bullet.h"
#include "Bootstrap.h"
#include "Logger.h"
#include "GameObjectManager.h"


Car::Car() : GameObject() {
	//initialize();
	//Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
}

void Car::fireBullet() {
    //Bullet* b = new Bullet()
    //std::shared_ptr<Bullet> b(new Bullet);
    //Shard::GameObjectManager::getInstance().addGameObject(b->shared_from_this());
    //Shard::Logger::log("FIRE");

    //// THIS SHOULD NOT BE DONE HERE,
    //// https://tenor.com/view/oh-no-oh-no-anyway-gif-18887547 
    //b->setupBullet(body_->trans->centre.x, body_->trans->centre.y);
    //b->body_->trans->rotate(body_->trans->rotz);
    //Shard::Bootstrap::getSound().playSound("fire.wav");
}

void Car::handleEvent(Shard::InputEvent ev, Shard::EventType et) {
    //Shard::Logger::log("handleEvent in ship");
   }

void Car::initialize() {

    setPhysicsEnabled(); // sets body_ to a new PhysicBody(this ) and populates transform_
    /*body_->trans->x = 500.f;
    body_->trans->y = 300.f;*/
    //auto path = Shard::Bootstrap::getAssetManager().getAssetPath("Car.png");
    //body_->trans->sprite_path = path;

    // if you move this stuff above transform_ init ^ then colliders will not be drawn
    // why? ... you figure it out!
	up = false;
	down = false;
	body_->mass = 1.f;
	//body_->max_force = 1000.f;
	//body_->angular_drag = 0.01f;
	body_->drag = 0.1f;
	body_->stop_on_collision = true;
	body_->reflect_on_collision = true;
	body_->impart_force = false;
	body_->is_kinematic = false;
    body_->pass_through = false;

    body_->setBoxCollider();
    //body_->addCircleCollider();

	model = std::make_shared<Shard::Model>("models/car.obj");
    
    GameObject::addTag("Car");
    Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
}

void Car::update() {
 
    //Shard::Bootstrap::getDisplay()->addToDraw(shared_from_this());
}

void Car::physicsUpdate() {
    //we need to change drag and the force/torque we add depending on the framerate
    auto frames = Shard::Bootstrap::getFPS();
    //50 frames: angular_drag = .01, drag = .1, torque = .3, force = .25/-.05
    //body_->angular_drag = frames / 3000;
    //body_->drag = frames / 500;

    //if (turn_left)
    //    body_->addTorque(-0.3f);
    //if (turn_right)
    //    body_->addTorque(0.3f);
    //if (up)
    //    body_->addForce(body_->trans->forward, 0.5f);
    //if (down)
    //    body_->addForce(body_->trans->forward, -0.2f);
}

void Car::prePhysicsUpdate() {
    // TODO: Should this be empty?
}

void Car::killMe() {
    // TODO: Clean up!!!
}
void Car::checkDestroyMe() {

 }

void Car::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {
    Shard::Logger::log("INDIDE ONCOLLISIONENTER Car");
    if (!body->parent->hasTag("Bullet"))
        body_->debug_color = { 1.0f, 0.0f, 0.0f };
    // TODO: Lower HP?
}

void Car::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {
    body_->debug_color = { 0, 1.0, 0.0f};
    // TODO: Not sure...
}

void Car::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {
    // TODO: Not sure...
    if (!body->parent->hasTag("Bullet"))
        body_->debug_color = { 0.0f, 0.0f, 1.0f };
}
