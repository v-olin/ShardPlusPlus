#include "GameObject.h"
#include "Transform3D.h"
#include "PhysicsBody.h"

#include <algorithm>

namespace Shard {
	GameObject::GameObject() {
		visible_ = false;

		to_be_destroyed_ = false;
		this->initialize();
	}

	void GameObject::addTag(std::string tag) {
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
		body_ = new PhysicsBody(this);
	}

	bool GameObject::queryPhysicsEnabled() {
		return body_ != nullptr;
	}

	void GameObject::checkDestroyMe() {
		if (!transient_)
			return;

		if (transform_.x > 0 && transform_.x < 1920 && // get width of display
			transform_.y > 0 && transform_.y < 1080) { // get height of display 
			return;
		}

		to_be_destroyed_ = true;
	}
}