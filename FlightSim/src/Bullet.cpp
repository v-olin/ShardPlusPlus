#include "Bullet.h"
#include "Bootstrap.h"
#include "Logger.h"
#include "GameObjectManager.h"
#include "Bootstrap.h"

#include <cstdlib>

Bullet::Bullet() : GameObject()
	, m_spawntime(0)
	, m_lifetime(20*1000)
	, lockedTarget(nullptr)
{
}

void Bullet::initialize() {
	//m_model = std::make_shared<Shard::Model>("models/bullet_best.obj");
    setPhysicsEnabled();
    m_body->m_mass = 1.f;
    m_body->m_maxForce = glm::vec3{ 400.f };
    m_body->m_angularDrag = glm::vec3{ 1.f };
    m_body->m_maxTorque = glm::vec3{ 100.0f, 100.0f, 100.0f };
	m_body->m_drag = 8.f;
	m_body->m_stopOnCollision = true;
	m_body->m_reflectOnCollision = true;
	m_body->m_impartForce = true;
	m_body->m_isKinematic = false;
    m_body->m_passThrough = true;
	m_body->m_usesGravity = false;
	m_drawCollider = false;

    m_body->m_bodyModel = m_model;
    m_body->setBoxCollider();
    
    GameObject::addTag("Bullet");
    Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
	m_spawntime = Shard::Bootstrap::getCurrentMillis();

}

void Bullet::physicsUpdate() {
	if (lockedTarget != nullptr && !lockedTarget->m_toBeDestroyed) {
		auto target = normalize(lockedTarget->m_model->position() + -m_model->position());
		m_model->m_forward = m_model->m_forward + (target - m_model->m_forward) / glm::vec3(40);
		m_model->m_right = glm::normalize(glm::cross(m_model->m_forward, m_model->m_up));
		m_model->m_up = glm::normalize(glm::cross(m_model->m_right, m_model->m_forward));
		m_model->m_rotMatrix = glm::mat4(
			glm::vec4(m_model->m_forward, 0),
			glm::vec4(m_model->m_up, 0),
			glm::vec4(m_model->m_right, 0),
			{ 0,0,0,1 });
	}
	//m_body->addForce(m_model->m_forward, 10.f);
}

void Bullet::update() {
}

void Bullet::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {
	if (!body->m_parent->hasTag("PlayerPlane") && !body->m_parent->hasTag("Bullet")) {
		Shard::Logger::log("Boom! (bullet ded)");
		m_toBeDestroyed = true;
	}
}
void Bullet::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {}
void Bullet::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {}
void Bullet::killMe() {
	Shard::Logger::log("Killing bullet");
}
void Bullet::prePhysicsUpdate(){}
void Bullet::checkDestroyMe() {
	if (Shard::Bootstrap::getCurrentMillis() - m_spawntime > m_lifetime)
		m_toBeDestroyed = true;
}


