#include "Car.h"
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
            forward = true;

        if (ev.key == GLFW_KEY_S)
            backward = true;

        if (ev.key == GLFW_KEY_A)
            turn_left = true;

        if (ev.key == GLFW_KEY_D)
            turn_right = true;

        if (ev.key == GLFW_KEY_LEFT_CONTROL)
            pitch_up = true;

        if (ev.key == GLFW_KEY_LEFT_SHIFT)
            pitch_down = true;
            
    }
    else if (et == Shard::EventType::KeyUp)
    {
        if (ev.key == GLFW_KEY_W)
            forward = false;

        if (ev.key == GLFW_KEY_S)
            backward = false;

        if (ev.key == GLFW_KEY_A)
            turn_left = false;

        if (ev.key == GLFW_KEY_D)
            turn_right = false;

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
    //auto path = Shard::Bootstrap::getAssetManager().getAssetPath("Car.png");
    //body_->trans->sprite_path = path;

    // if you move this stuff above transform_ init ^ then colliders will not be drawn
    // why? ... you figure it out!
	forward = false;
	backward = false;
    turn_left = false;
    turn_right = false;
    pitch_down = false;
    pitch_up = false;
	m_body->m_mass = 1.f;
    m_body->m_maxForce = glm::vec3{ 0.2f };
    m_body->m_angularDrag = glm::vec3{ 0.01f };
    m_body->m_maxTorque = glm::vec3{ 10.0f, 10.0f, 10.0f };
	m_body->m_drag = 0.1f;
	m_body->m_stopOnCollision = true;
	m_body->m_reflectOnCollision = true;
	m_body->m_impartForce = true;
	m_body->m_isKinematic = false;
    m_body->m_passThrough = false;
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
    
    GameObject::addTag("Car");
    Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
    //Shard::Bootstrap::getInput().addListeners(shared_from_this());
}

void PlayerPlane::update() {
 
    //Shard::Bootstrap::getDisplay()->addToDraw(shared_from_this());
}


void PlayerPlane::physicsUpdate() {
    if (!should_move)
        return;
    if (turn_left)
        m_body->addTorque({ 0, 0.02f, 0 });
    if (turn_right)
        m_body->addTorque({ 0, -0.02f, 0 });
    if (backward)
        m_body->addForce(m_model->m_forward, 0.1f);
    if (forward)
        m_body->addForce(m_model->m_forward, -0.1f);
    if (pitch_up)
        m_body->addTorque({ 0, 0, 0.02f });
    if (pitch_down)
        m_body->addTorque({ 0, 0, -0.02f });
    //restart camera
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
