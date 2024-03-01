#include "PLayerPlane.h"
#include "Logger.h"
#include "Bullet.h"
#include "Bootstrap.h"
#include "Logger.h"
#include "GameObjectManager.h"


PlayerPlane::PlayerPlane() : GameObject() {
	//initialize();
	//Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
}

void PlayerPlane::fireBullet() {
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

void PlayerPlane::handleEvent(Shard::InputEvent ev, Shard::EventType et) {

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
            pitch_up = true;
        if (ev.key == GLFW_KEY_LEFT_SHIFT)
            pitch_down = true;
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
            pitch_up = false;
        if (ev.key == GLFW_KEY_LEFT_SHIFT)
            pitch_down = false;
    }
}

void PlayerPlane::initialize() {
    m_model = std::make_shared<Shard::Model>("models/space-ship.obj");

    setPhysicsEnabled(); // sets body_ to a new PhysicBody(this ) and populates transform_
    /*body_->trans->x = 500.f;
    body_->trans->y = 300.f;*/
    //auto path = Shard::Bootstrap::getAssetManager().getAssetPath("PlayerPlane.png");
    //body_->trans->sprite_path = path;

    // if you move this stuff above transform_ init ^ then colliders will not be drawn
    // why? ... you figure it out!
	throttle = 0;
    throttle_change = 0.1f;
    roll_left = false;
    roll_right = false;
    pitch_down = false;
    pitch_up = false;
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
    m_body->m_usesGravity = false;
    m_body->m_bodyModel = m_model;

    /*
        Short story:
            A car has a physicsbody and a model
            But the physicsbody also has a pointer to the same model
            The physicsbody also has a collider WHICH ALSO HAS A POINTER TO THE SAME MODEL
            so the dependencies and order is fucked
            this specific order is the only correct initialization
            someone please fix this
            i hate this
            god help me
    */
    m_body->setBoxCollider();
    
    GameObject::addTag("PlayerPlane");
    Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
    //Shard::Bootstrap::getInput().addListeners(shared_from_this());
}

void PlayerPlane::update() {
 
    //Shard::Bootstrap::getDisplay()->addToDraw(shared_from_this());
}


void PlayerPlane::physicsUpdate() {
    if (yaw_left)
        m_body->addTorque({ 0, 2.f, 0 });
    if (yaw_right)
        m_body->addTorque({ 0, -2.f, 0 });
    if (pitch_up)
        m_body->addTorque({ 0, 0, 2.f });
    if (pitch_down)
        m_body->addTorque({ 0, 0, -2.f });
    if (roll_left)
        m_body->addTorque({ -2.f, 0, 0 });
    if (roll_right)
        m_body->addTorque({ 2.f, 0, 0 });
    if (throttle_forward)
        m_body->addForce(m_model->m_forward, 100.0);
    if (throttle_back)
        m_body->addForce(m_model->m_forward, -10.0);
    //calculate lift here and apply force in m_up

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
