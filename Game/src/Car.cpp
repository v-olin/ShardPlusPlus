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

    if (et == Shard::EventType::KeyDown)
    {
        if (ev.key == GLFW_KEY_W)
            up = true;

        if (ev.key == GLFW_KEY_S)
            down = true;

        if (ev.key == GLFW_KEY_A)
            turn_left = true;

        if (ev.key == GLFW_KEY_D)
            turn_right = true;

    }
    else if (et == Shard::EventType::KeyUp)
    {
        if (ev.key == GLFW_KEY_W)
            up = false;

        if (ev.key == GLFW_KEY_S)
            down = false;

        if (ev.key == GLFW_KEY_A)
            turn_left = false;

        if (ev.key == GLFW_KEY_D)
            turn_right = false;
    }

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
	m_body->m_mass = 1.f;
    m_body->m_maxForce = glm::vec3{ 0.2f };
    m_body->m_angularDrag = glm::vec3{ 0.01f };
	m_body->m_drag = 0.1f;
	m_body->m_stopOnCollision = true;
	m_body->m_reflectOnCollision = true;
	m_body->m_impartForce = false;
	m_body->m_isKinematic = false;
    m_body->m_passThrough = false;

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
    m_model = std::make_shared<Shard::Model>("models/space-ship.obj");
    m_body->m_bodyModel = m_model;
    m_body->setBoxCollider();
    
    GameObject::addTag("Car");
    Shard::GameObjectManager::getInstance().addGameObject(shared_from_this());
    //Shard::Bootstrap::getInput().addListeners(shared_from_this());
}

void Car::update() {
 
    //Shard::Bootstrap::getDisplay()->addToDraw(shared_from_this());
}

void Car::physicsUpdate() {
    if (turn_left)
        m_body->addTorque({ 0.0f, 0.1f, 0.0f });
    if (turn_right)
        m_body->addTorque({ 0.0f, -0.1f, 0.0f });
    if (up)
        m_body->addForce(m_model->m_forward, 0.1f);

    if (down)
        m_body->addForce(m_model->m_forward, -0.1f);
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
    if (!body->m_parent->hasTag("Bullet"))
        m_body->m_debugColor = { 1.0f, 0.0f, 0.0f };
    // TODO: Lower HP?
}

void Car::onCollisionExit(std::shared_ptr<Shard::PhysicsBody> body) {
    m_body->m_debugColor = { 0, 1.0, 0.0f};
    // TODO: Not sure...
}

void Car::onCollisionStay(std::shared_ptr<Shard::PhysicsBody> body) {
    // TODO: Not sure...
    if (!body->m_parent->hasTag("Bullet"))
        m_body->m_debugColor = { 0.0f, 0.0f, 1.0f };
}
