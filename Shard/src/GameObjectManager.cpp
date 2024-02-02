#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "Bootstrap.h"

namespace Shard{

	void GameObjectManager::addGameObject(GameObject* obj){
		this->myObjects.push_back(obj);
	}

	void GameObjectManager::removeGameObject(GameObject* obj){
		auto iter = this->myObjects.begin();
		while (++iter != this->myObjects.end()) {
			if (obj->body_ == (*iter)->body_) {
				this->myObjects.erase(iter);
				return;
			}
		}
	}

	void GameObjectManager::physicsUpdate(){

		for (auto& gob : this->myObjects)
			gob->physicsUpdate();

		/*
		auto iter = getInstance()->myObjects.begin();
		while (iter != getInstance()->myObjects.end()) 
			(*iter++).physicsUpdate();
		*/

	}

	void GameObjectManager::prePhysicsUpdate(){
		if (getInstance()->myObjects.size() == 0)
			return;
		auto iter = getInstance()->myObjects.begin();
		while (iter != getInstance()->myObjects.end()) {
			(*iter++)->prePhysicsUpdate();
		}

	}

	void GameObjectManager::update() {

		std::vector<Shard::GameObject*> to_be_deleted;
		for (auto& gob : this->myObjects) {
			gob->update();
			gob->checkDestroyMe();
			if (gob->to_be_destroyed_)
				to_be_deleted.push_back(gob);
		}
		for (auto& gob : to_be_deleted) {
			PhysicsManager::getInstance().removePhysicsObject(gob->body_);

			InputListener* t;
			if (t = dynamic_cast<InputListener*>(gob))
				Bootstrap::getInput()->removeListeners(t);
			gob->killMe();
			std::erase(myObjects, gob); // inefficient af.

			// we need to make sure that the physicsmanager knows that we have deleted some gameObjects
			// to do this, start with tracking from creation of game object and list all places where the pointer to that object is stored
			// we need to remove those pointers ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			// And this is fine since this function is run first in every frame
			delete gob;
		}


		// erase-remove idiom, TODO: we also need to call killMe() on all objects that is goinmg to be deleted
		//TODO, we need to delete the obects, since this might leek memory
		//this->myObjects.erase(std::remove_if(this->myObjects.begin(), this->myObjects.end(), [&](const Shard::GameObject* obj) {
		//	return std::find(to_be_deleted.begin(), to_be_deleted.end(), obj) != to_be_deleted.end();
		//	}), this->myObjects.end());

	}

}