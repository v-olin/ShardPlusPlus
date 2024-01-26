#include "GameObjectManager.h"
namespace Shard{
	void GameObjectManager::addGameObject(GameObject obj){
		getInstance().myObjects.push_back(obj);
	}
	void GameObjectManager::removeGameObject(GameObject obj){
		auto iter = getInstance().myObjects.begin();

		while (++iter != getInstance().myObjects.end()) {
			if (obj.body_ == (*iter).body_)
				getInstance().myObjects.erase(iter);
			return;
		}
	}

	void GameObjectManager::physicsUpdate(){
		auto iter = getInstance().myObjects.begin();
		while (++iter != getInstance().myObjects.end()) 
			(*iter).physicsUpdate();
	}

	void GameObjectManager::prePhysicsUpdate(){
		auto iter = getInstance().myObjects.begin();
		while (++iter != getInstance().myObjects.end()) 
			(*iter).prePhysicsUpdate();
	}

	void GameObjectManager::update(){
		auto iter = getInstance().myObjects.begin();
		GameObject gob;
		while (++iter != getInstance().myObjects.end()) {
			gob = (*iter);
			gob.update();
			gob.checkDestroyMe();
			if (gob.to_be_destroyed_)
				getInstance().myObjects.erase(iter);
		}
	}

}