#pragma once

#include "GameObject.h"

#include <vector>
namespace Shard {
	class  GameObjectManager {
	public:
		 void addGameObject(GameObject* obj);
		 void removeGameObject(GameObject* obj);
		 void physicsUpdate();
		 void prePhysicsUpdate();
		 void update();
		 void cleanup();

		static GameObjectManager* getInstance(){
			static GameObjectManager manager;
			return &manager;
		}


	private:
		std::vector<GameObject*> myObjects;
		std::vector<GameObject*> to_be_deleted;
	
	}; 
}
