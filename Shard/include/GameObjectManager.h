#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif


#include "GameObject.h"

#include <vector>
namespace Shard {
	class  GameObjectManager {
	public:
		SHARD_API void addGameObject(GameObject obj);
		SHARD_API void removeGameObject(GameObject obj);
		SHARD_API void physicsUpdate();
		SHARD_API void prePhysicsUpdate();
		SHARD_API void update();

		static SHARD_API GameObjectManager getInstance(){
			static GameObjectManager manager;
			return manager;
		}


	private:
		std::vector<GameObject> myObjects;
	
	}; 
}
