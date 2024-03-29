#include "PLayerPlane.h"
#include "Logger.h"
#include "Bullet.h"
#include "Bootstrap.h"
#include "Logger.h"
#include "GameObjectManager.h"


PlayerPlane::PlayerPlane() : GameObject() { }

void PlayerPlane::handleEvent(Shard::InputEvent ev, Shard::EventType et) {

    if (*rmb_down)
        return;
    if (et == Shard::EventType::KeyDown)
    {
        if (ev.key == GLFW_KEY_W)
            throttle_forward = true;
        if (ev.key == GLFW_KEY_S)
            throttle_back = true;
        if (ev.key == GLFW_KEY_A)
            roll_left = true;
        if (ev.key == GLFW_KEY_D)
            roll_right = true;
        if (ev.key == GLFW_KEY_Q)
            yaw_left = true;
        if (ev.key == GLFW_KEY_E)
            yaw_right = true;
        if (ev.key == GLFW_KEY_LEFT_CONTROL)
            height_down = true;
        if (ev.key == GLFW_KEY_LEFT_SHIFT)
            height_up = true;
    }
    else if (et == Shard::EventType::KeyUp)
    {
        if (ev.key == GLFW_KEY_W)
            throttle_forward = false;
        if (ev.key == GLFW_KEY_S)
            throttle_back = false;
        if (ev.key == GLFW_KEY_A)
            roll_left = false;
        if (ev.key == GLFW_KEY_D)
            roll_right = false;
        if (ev.key == GLFW_KEY_Q)
            yaw_left = false;
        if (ev.key == GLFW_KEY_E)
            yaw_right = false;
        if (ev.key == GLFW_KEY_LEFT_CONTROL)
            height_down = false;
        if (ev.key == GLFW_KEY_LEFT_SHIFT)
            height_up = false;
    }
}

void PlayerPlane::initialize() {
    m_model = std::make_shared<Shard::Model>("models/chopper.obj");
    setPhysicsEnabled(); // sets body_ to a new PhysicBody(this ) and populates transform_

    // if you move this stuff above transform_ init ^ then colliders will not be drawn
    // why? ... you figure it out!
	throttle = 0;
    throttle_change = 0.1f;
    roll_left = false;
    roll_right = false;
    height_down = false;
    height_up = false;
    yaw_left = false;
    yaw_right = false;
	m_body->m_mass = 100.f;
    m_body->m_maxForce = glm::vec3{ 200.f };
    m_body->m_angularDrag = glm::vec3{ 1.f };
    m_body->m_maxTorque = glm::vec3{ 1000.0f, 1000.0f, 1000.0f };
	m_body->m_drag = 0.1f;
	m_body->m_stopOnCollision = true;
	m_body->m_reflectOnCollision = true;
	m_body->m_impartForce = true;
	m_body->m_isKinematic = false;
    m_body->m_passThrough = false;
    m_body->m_usesGravity = true;
    m_body->m_clickable = false;
    m_body->m_bodyModel = m_model;

    /*
        Short story:
            A car has a physicsbody and a model
            But the physicsbody also has a pointer to the same model
            The physicsbody also has a collider WHICH ALSO HAS A POINTER TO THE SAME MODEL
            so the dependencies and order is borked
            this specific order is the only correct initialization
            someone please fix this
            i hate this
            god help me

            thanks faton for fix :^)
    */
    m_body->setBoxCollider();
    
    GameObject::addTag("PlayerPlane");
    Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
}

void PlayerPlane::update() { }


void PlayerPlane::physicsUpdate() {

    float up_force = 100;
    if (yaw_left)
        m_body->addTorque({ 0, 2.f, 0 });
    if (yaw_right)
        m_body->addTorque({ 0, -2.f, 0 });
    if (height_up)
        up_force += 50;
    if (height_down)
        up_force -= 50;
    auto max_roll = 45 * (3.14159265358979 / 180);
    auto rot_x = m_model->rotation()[0];
    if (roll_left && rot_x > -max_roll)
        m_body->addTorque({ -2.f, 0, 0 });
    if (roll_right && rot_x < max_roll)
        m_body->addTorque({ 2.f, 0, 0 });
    float rot_z = m_model->rotation()[2];
    auto max_pitch = 30 * (3.14159265358979 / 180);
    if (throttle_forward&& rot_z >  -max_pitch) {
        m_body->addTorque({ 0,0, -2 });
        m_body->addForce({ 0, 1, 0 }, 20);
    }
    if (throttle_back && rot_z < max_pitch) {
        m_body->addTorque({ 0,0, 2 });
        m_body->addForce({ 0, 1, 0 }, 20);
     
    }

    m_body->addForce(m_model->m_up, up_force);
}

void PlayerPlane::prePhysicsUpdate() {
    // TODO: Should this be empty?
}

void PlayerPlane::killMe() {
    // TODO: Clean up!!!
}
void PlayerPlane::checkDestroyMe() {

 }

void PlayerPlane::onCollisionEnter(std::shared_ptr<Shard::PhysicsBody> body) {
    Shard::Logger::log("on collsision ENTER CAR");
    if (!body->m_parent->hasTag("Bullet")) {
        m_body->m_debugColor = { 1.0f, 0.0f, 0.0f };
    }
    // TODO: Lower HP?
}

void PlayerPlane::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {
    Shard::Logger::log("on collsision EXIT CAR");
    m_body->m_debugColor = { 0, 1.0, 0.0f};
    // TODO: Not sure...
}

void PlayerPlane::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {
    // TODO: Not sure...
    Shard::Logger::log("on collsision STAY CAR");
    if (!body->m_parent->hasTag("Bullet"))
        m_body->m_debugColor = { 0.0f, 0.0f, 1.0f };
}
