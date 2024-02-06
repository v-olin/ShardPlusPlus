#include "GameObject.h"
#include "Transform3D.h"
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
		body_ = std::make_shared<PhysicsBody>();
		PhysicsManager::getInstance().addPhysicsObject(body_->shared_from_this());
	}

	bool GameObject::queryPhysicsEnabled() {
		return body_ != nullptr;
	}

	void GameObject::checkDestroyMe() {
		if (!transient_)
			return;

		auto& transform = body_->trans;

		SDL_DisplayMode dm;
		SDL_GetCurrentDisplayMode(0, &dm);
		auto display_width = dm.w;
		auto display_height = dm.h;

		if (transform->x > 0 && transform->x < display_width && // get width of display
			transform->y > 0 && transform->y < display_height) { // get height of display 
			return;
		}

		to_be_destroyed_ = true;
	}
}