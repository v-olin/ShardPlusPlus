//#include "Building.h"
//#include "Bootstrap.h"
//#include "Logger.h"
//#include "GameObjectManager.h"
//#include "Bootstrap.h"
//
////#include <cstdlib>
//#include <cstring>
//
//Building::Building() : GameObject()
//{
//}
//
//void Building::initialize() {
//
//
//	
//
//	auto num = 1 + (rand() % 9);
//	auto start = num != 10 ? "Residential Buildings 00" : "Residential Buildings 0";
//	auto name = start + std::to_string(num) + ".obj";
//	//Residential Buildings 010.obj
//
//	m_model = std::make_shared<Shard::Model>("models/city/");
//	setPhysicsEnabled();
//	m_body->m_mass = 10000000000.f;
//	m_body->m_maxForce = glm::vec3{ 0.f };
//	m_body->m_angularDrag = glm::vec3{ 10000.f };
//	m_body->m_maxTorque = glm::vec3{0};
//	m_body->m_drag = 100000.f;
//	m_body->m_stopOnCollision = true;
//	m_body->m_reflectOnCollision = true;
//	m_body->m_impartForce = true;
//	m_body->m_isKinematic = false;
//	m_body->m_passThrough = false;
//	m_body->m_usesGravity = false;
//	m_body->m_clickable = false;
//
//	m_body->m_bodyModel = m_model;
//	m_body->setBoxCollider();
//
//	GameObject::addTag("Building");
//	Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
//
//}
//
//void Building::physicsUpdate() {
//}
//void Building::update() {
//}
//
//void Building::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {
//}
//void Building::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {}
//void Building::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {}
//void Building::killMe() {
//	Shard::Logger::log("Killing Building");
//}
//void Building::prePhysicsUpdate() {}
//void Building::checkDestroyMe() {
//}

