#include "AIPlane.h"
#include "GameObjectManager.h"
#include "Bootstrap.h"
#include "Logger.h"
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/string_cast.hpp"
#include "common.h"

void handleAIPLANEInput(Shard::InputEvent ev, Shard::EventType et) {

}

AIPlane::AIPlane() : GameObject()
    , target_hunt_time(10*1000)
    , m_hunt_start_time(0)
    , m_hunt_index(0)
    {
    generateTargetPonts();
}

void AIPlane::generateTargetPonts() {
	int max = 10000;
    int ymax = 2000;
    for (int i = 0; i < 40; i++) {
		auto pos = glm::vec3(rand() % max, rand() % ymax, rand() % max) - glm::vec3(max/2, 0, max/2);
        target_points.push_back({pos.x, pos.y, pos.z});
    }

}

void AIPlane::handleEvent(Shard::InputEvent ie, Shard::EventType et) { }

void AIPlane::initialize()
{
    setPhysicsEnabled();
    m_body->m_mass = 1.f;
    m_body->m_maxForce = glm::vec3{ 100.f };
    m_body->m_angularDrag = glm::vec3{ 0.02f };
    m_body->m_maxTorque = glm::vec3{ 0.4 };
    m_body->m_drag = .1f;
    m_body->m_stopOnCollision = true;
    m_body->m_reflectOnCollision = true;
    m_body->m_impartForce = true;
    m_body->m_isKinematic = false;
    m_body->m_passThrough = false;
    m_body->m_usesGravity = false;

    m_body->m_bodyModel = m_model;
    m_body->setBoxCollider();

    GameObject::addTag("AIPlane");
    Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
	m_hunt_start_time = Shard::Bootstrap::getCurrentMillis();

}
void AIPlane::update() {
    auto now = Shard::Bootstrap::getCurrentMillis();
    if (now - m_hunt_start_time >= target_hunt_time) {
        m_hunt_index = (m_hunt_index + 1) % target_points.size();
        m_hunt_start_time = now;
    }
}

void AIPlane::physicsUpdate() {
  
	auto target = glm::normalize(target_points.at(m_hunt_index) - m_model->position());
    auto pos = m_model->position();
    auto len = glm::length(target_points.at(m_hunt_index) - m_model->position());
    if (len < 100) {
        m_hunt_index = (m_hunt_index + 1) % target_points.size();
        m_hunt_start_time = Shard::Bootstrap::getCurrentMillis();
        return;
    }
       
    auto dir =  m_model->m_forward + (target - m_model->m_forward) / glm::vec3(100);
    m_model->m_forward = glm::normalize(glm::vec3{ dir.x, m_model->m_forward.y, dir.z });
	m_model->m_right = glm::normalize(glm::cross(m_model->m_forward, m_model->m_up));
	m_model->m_up = glm::normalize(glm::cross(m_model->m_right, m_model->m_forward));
    m_model->m_rotMatrix = glm::mat4(
        glm::vec4(m_model->m_forward, 0),
        glm::vec4(m_model->m_up, 0),
        glm::vec4(m_model->m_right, 0),
        { 0,0,0,1 });

    m_body->addForce(target, 100.f);
}

void AIPlane::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {

    m_body->m_debugColor = { 1.0f, 0.0f, 0.0f };
    Shard::Logger::log("on collsision ENTER AIPLANE");

    if (body->m_parent->hasTag("Bullet") || body->m_parent->hasTag("House")) {
        m_toBeDestroyed = true;
        Shard::Logger::log("Boom!");
    }

}

void AIPlane::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {
    Shard::Logger::log("on collsision EXIT AIPLANE");
    m_body->m_debugColor = { 0.0f, 1.0f, 0.0f };
}

void AIPlane::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {
    Shard::Logger::log("on collsision STAY AIPLANE");
    m_body->m_debugColor = { 0.0f, 0.0f, 1.0f };
}

void AIPlane::killMe() { }
void AIPlane::prePhysicsUpdate() { }
void AIPlane::checkDestroyMe() { }
