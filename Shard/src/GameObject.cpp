#include "GameObject.h"
#include "PhysicsBody.h"
#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "Logger.h"
#include <algorithm>

namespace Shard {
	GameObject::GameObject()
		: tags({})
		, transient_(false)
		, to_be_destroyed_(false)
		, visible_(false)
		, body_(nullptr) 
	{
	}

	void GameObject::addTag(const char* tag) {
		tags.push_back(tag);
	}

	void GameObject::removeTag(std::string tag) {
		auto iter = std::find(tags.begin(), tags.end(), tag);

		if (iter != tags.end())
			tags.erase(iter);
	}

	bool GameObject::hasTag(std::string tag) {
		auto iter = std::find(tags.begin(), tags.end(), tag);

		return iter != tags.end();
	}

	std::string GameObject::getTags() {
		std::string concat = "";

		for (std::string s : tags) {
			concat += s + ";";
		}

		return concat;
	}

	void GameObject::setPhysicsEnabled() {
		body_ = std::make_shared<PhysicsBody>(shared_from_this());
		body_->setBoxCollider();
		PhysicsManager::getInstance().addPhysicsObject(body_->shared_from_this());
	}

	bool GameObject::queryPhysicsEnabled() {
		return body_ != nullptr;
	}

}