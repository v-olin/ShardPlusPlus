#include "Bullet.h"
#include "Bootstrap.h"
#include "Logger.h"
#include "GameObjectManager.h"
#include "Bootstrap.h"

#include <cstdlib>

Bullet::Bullet() : GameObject()
	,m_spawntime(0)
	,m_lifetime(3*1000)
{
}

void Bullet::initialize() {
	m_model = std::make_shared<Shard::Model>("models/bullet_best.obj");
	//m_model->m_forward = glm::vec3(-1, 0, 0);
    setPhysicsEnabled();
    m_body->m_mass = 1.f;
    m_body->m_maxForce = glm::vec3{ 2.f };
    m_body->m_angularDrag = glm::vec3{ 0.01f };
    m_body->m_maxTorque = glm::vec3{ 10.0f, 10.0f, 10.0f };
	m_body->m_drag = 1.f;
	m_body->m_stopOnCollision = true;
	m_body->m_reflectOnCollision = true;
	m_body->m_impartForce = true;
	m_body->m_isKinematic = false;
    m_body->m_passThrough = true;

    m_body->m_bodyModel = m_model;
    m_body->setBoxCollider();
    
    GameObject::addTag("Bullet");
    Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
	m_spawntime = Shard::Bootstrap::getCurrentMillis();

}

void Bullet::physicsUpdate() {
	//fucking car is reversed, very bad!!!
	m_body->addForce(m_model->m_forward, -1.f);

    //restart camera
}

void Bullet::update() {
	//unsigned char r = rand() % 256, g = rand() % 256;
	//SDL_Color color = { r, g, 0, 255 };

	//Shard::Display* disp = Shard::Bootstrap::getDisplay();

	//disp->drawLine(
	//	body_->trans->x,
	//	body_->trans->y,
	//	body_->trans->x + 10,
	//	body_->trans->y + 10,
	//	color
	//);

	//disp->drawLine(
	//	body_->trans->x + 10,
	//	body_->trans->y,
	//	body_->trans->x,
	//	body_->trans->y + 10,
	//	color
	//);
}

void Bullet::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {
	if (body->m_parent->hasTag("Car") == false && body->m_parent->hasTag("Bullet") == false) {
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


