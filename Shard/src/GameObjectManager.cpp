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
	}

	void GameObjectManager::cleanup() {
		for (auto& gob : to_be_deleted) {
			PhysicsManager::getInstance().removePhysicsObject(gob->m_body);

			std::shared_ptr<InputListener> listener =
				std::dynamic_pointer_cast<InputListener>(gob);
			if (listener) // if successful UPCAST
				Bootstrap::getInput().removeListeners(listener);
			listener = nullptr;
			gob->killMe();
			std::erase(myObjects, gob);
			gob->m_body = nullptr;
		}
		to_be_deleted.clear();
	}
}