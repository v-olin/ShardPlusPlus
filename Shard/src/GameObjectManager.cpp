#include "GameObjectManager.h"

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

		// erase-remove idiom
		this->myObjects.erase(std::remove_if(this->myObjects.begin(), this->myObjects.end(), [&](const Shard::GameObject* obj) {
			return std::find(to_be_deleted.begin(), to_be_deleted.end(), obj) != to_be_deleted.end();
			}), this->myObjects.end());

	}

}