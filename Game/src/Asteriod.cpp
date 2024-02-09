//#include "Asteriod.h"
//#include "GameObjectManager.h"
//#include "Bootstrap.h"
//#include "Logger.h"
//#include <memory>
//
//void handleAsteroidInput(Shard::InputEvent ev, Shard::EventType et) {
//
//}
//
//Asteroid::Asteroid() : GameObject() {
//	
//}
//
//
//void Asteroid::handleEvent(Shard::InputEvent ie, Shard::EventType et) {
//    if (et == Shard::EventType::MouseDown && ie.button == SDL_BUTTON_MIDDLE) {
//        if ((body_->checkCollisions(glm::vec2{ ie.x, ie.y })).has_value()) {
//            torque_counter += 10;
//        }
//    }
//}
//
//void Asteroid::initialize()
//{
//    setPhysicsEnabled();
//    body_->trans->x = 300.0f;
//	body_->trans->y = 300.0f;
//	auto path = Shard::Bootstrap::getAssetManager().getAssetPath("asteroid.png");
//	body_->trans->sprite_path = path;
//
//    body_->max_torque = 50;
//    body_->max_force = 100;
//    body_->mass = 1;
//    body_->uses_gravity = true;
//    body_->stop_on_collision = false;
//    body_->reflect_on_collision = true;
//    body_->is_kinematic = false;
//    //body_->pass_through = false;
//    
//    // TODO: Add randomness to initial force
//    //body_->addForce(body_->trans.right, 2.0f);
//
//    body_->addRectCollider();
//
//    /*
//    
//    getSharedPtr() -> std::shared_ptr<GameBoject>
//    asteroid->getSharedPtr();
//    std::shared_ptr<GameObject>
//
//    */
//
//    Shard::Bootstrap::getInput().addListeners(
//        // shared_ptr<GOBJ> (ptr -> asteroid (which is listener))
//        //shared_from_this()
//        std::dynamic_pointer_cast<Shard::InputListener>(shared_from_this())
//    );
//
//    GameObject::addTag("Asteroid");
//
//}
//void Asteroid::update(){
//    Shard::Bootstrap::getDisplay()->addToDraw(shared_from_this());
//}
//
//void Asteroid::physicsUpdate() {
//    for (int i = 0; i < torque_counter; i++)
//        body_->addTorque(0.1f);
//
//    if (torque_counter > 0)
//        torque_counter -= 1;
//}
//
//void Asteroid::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {
//
//    Shard::Logger::log("INSIDE ONCOLLISIONENTER ASTEROOOOOOOOID@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", Shard::LoggerLevel::LOG_LEVEL_ALL);
//
//    if (body->parent->hasTag("Bullet")) {
//        to_be_destroyed_ = true;
//		Shard::Logger::log("Boom!");
//    }
//
//    Shard::Logger::log("Bang!");
//}
//
//void Asteroid::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {
//    Shard::Logger::log("Anti bang!");
//}
//
//void Asteroid::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) { }
//void Asteroid::killMe(){
//}
//void Asteroid::prePhysicsUpdate(){}
