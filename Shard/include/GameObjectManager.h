#pragma once

#include "GameObject.h"
#include <vector>

namespace Shard {
	class  GameObjectManager {
	public:
		 void addGameObject(std::shared_ptr<GameObject> obj);
		 void removeGameObject(std::shared_ptr<GameObject> obj);
		 void physicsUpdate();
		 void prePhysicsUpdate();
		 void update();
		 void cleanup();

		 std::vector<std::shared_ptr<GameObject>>& getObjects();
		static GameObjectManager& getInstance(){
			static GameObjectManager manager;
			return manager;
		}

		GameObjectManager(GameObjectManager const&) = delete;
		void operator=(GameObjectManager const&) = delete;

	private:
		GameObjectManager() = default;
		std::vector<std::shared_ptr<GameObject>> myObjects;
		std::vector<std::shared_ptr<GameObject>> to_be_deleted;
	
	}; 
}
