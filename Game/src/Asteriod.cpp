#include "Asteriod.h"
#include "GameObjectManager.h"
#include "Bootstrap.h"
#include "Logger.h"
#include <memory>

void handleAsteroidInput(Shard::InputEvent ev, Shard::EventType et) {

}

Asteroid::Asteroid() : GameObject() {
	
}


void Asteroid::handleEvent(Shard::InputEvent ie, Shard::EventType et) {
    /*if (et == Shard::EventType::MouseDown && ie.button == GLFW_KE) {
        if ((m_body->checkCollisions(glm::vec2{ ie.x, ie.y })).has_value()) {
            torque_counter += 10;
        }
    }*/
}

void Asteroid::initialize()
{
    m_model = std::make_shared<Shard::Model>("models/cube.obj");
    setPhysicsEnabled();
    m_body->m_mass = 10.f;
    m_body->m_maxForce = glm::vec3{ 0.2f };
    m_body->m_angularDrag = glm::vec3{ 0.01f };
    m_body->m_maxTorque = glm::vec3{ 10.0f, 10.0f, 10.0f };
	m_body->m_drag = 0.1f;
	m_body->m_stopOnCollision = true;
	m_body->m_reflectOnCollision = true;
	m_body->m_impartForce = false;
	m_body->m_isKinematic = false;
    m_body->m_passThrough = false;

    m_body->m_bodyModel = m_model;
    m_body->setBoxCollider();
    
    GameObject::addTag("Asteriod");
    Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());

}
void Asteroid::update(){
}

void Asteroid::physicsUpdate() {
  }

void Asteroid::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {

       m_body->m_debugColor = { 1.0f, 0.0f, 0.0f };
    Shard::Logger::log("on collsision ENTER ASTEROID");

  /*  if (body->m_parent->hasTag("Bullet")) {
        to_be_destroyed_ = true;
		Shard::Logger::log("Boom!");
    }*/

}

void Asteroid::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {
    Shard::Logger::log("on collsision EXIT ASTEROID");
        m_body->m_debugColor = { 0.0f, 1.0f, 0.0f };
}

void Asteroid::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {
    Shard::Logger::log("on collsision STAY ASTEROID");
        m_body->m_debugColor = { 0.0f, 0.0f, 1.0f };
}

void Asteroid::killMe(){
}
void Asteroid::prePhysicsUpdate(){}
void Asteroid::checkDestroyMe() {

 }

