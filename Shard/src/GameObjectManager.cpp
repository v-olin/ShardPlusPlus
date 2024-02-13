#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "Bootstrap.h"

namespace Shard{

	void GameObjectManager::addGameObject(std::shared_ptr<GameObject> obj){
		this->myObjects.push_back(obj);
	}
	
	void GameObjectManager::removeGameObject(std::shared_ptr<GameObject> obj){
		auto iter = this->myObjects.begin();
		while (++iter != this->myObjects.end()) {
			if (obj->m_body == (*iter)->m_body) {
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
		if (getInstance().myObjects.size() == 0)
			return;
		auto iter = getInstance().myObjects.begin();
		while (iter != getInstance().myObjects.end()) {
			(*iter++)->prePhysicsUpdate();
		}

	}

	
	std::vector<std::shared_ptr<GameObject>>& GameObjectManager::getObjects() {
		return myObjects;
	}
	void GameObjectManager::update() {

		for (auto& gob : this->myObjects) {
			gob->update();
			gob->checkDestroyMe();
			if (gob->m_toBeDestroyed)
				to_be_deleted.push_back(gob);
		}


		// erase-remove idiom, TODO: we also need to call killMe() on all objects that is goinmg to be deleted
		//TODO, we need to delete the obects, since this might leek memory
		//this->myObjects.erase(std::remove_if(this->myObjects.begin(), this->myObjects.end(), [&](const Shard::GameObject* obj) {
		//	return std::find(to_be_deleted.begin(), to_be_deleted.end(), obj) != to_be_deleted.end();
		//	}), this->myObjects.end());

	}

	void GameObjectManager::cleanup() {
		for (auto& gob : to_be_deleted) {
			PhysicsManager::getInstance().removePhysicsObject(gob->m_body);

			std::shared_ptr<InputListener> listener =
				std::dynamic_pointer_cast<InputListener>(gob);
			if (listener) // if successful downcast
				Bootstrap::getInput().removeListeners(listener);
			listener = nullptr;
			gob->killMe();
			std::erase(myObjects, gob); // inefficient af.
			gob->m_body = nullptr;

			// we need to make sure that the physicsmanager knows that we have deleted some gameObjects
			// to do this, start with tracking from creation of game object and list all places where the pointer to that object is stored
			// we need to remove those pointers ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			// And this is fine since this function is run first in every frame
			
			// we shouldn't need to delete gob here since it is now a shared_ptr
			// which should delete itself (if we're lucky)
			// delete gob; 
		}
		to_be_deleted.clear();
	}
}