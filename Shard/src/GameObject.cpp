#include "GameObject.h"
#include "PhysicsBody.h"
#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "Logger.h"
#include <algorithm>

namespace Shard {
	GameObject::GameObject()
		: m_tags({})
		, m_transient(false)
		, m_toBeDestroyed(false)
		, m_visible(false)
		, m_body(nullptr)
		, m_model(nullptr)
	{
	}

	void GameObject::addTag(const char* tag) {
		m_tags.push_back(tag);
	}

	void GameObject::removeTag(std::string tag) {
		auto iter = std::find(m_tags.begin(), m_tags.end(), tag);

		if (iter != m_tags.end())
			m_tags.erase(iter);
	}

	bool GameObject::hasTag(std::string tag) {
		auto iter = std::find(m_tags.begin(), m_tags.end(), tag);

		return iter != m_tags.end();
	}

	std::string GameObject::getTags() {
		std::string concat = "";

		for (std::string s : m_tags) {
			concat += s + ";";
		}

		return concat;
	}

	void GameObject::setPhysicsEnabled() {
		m_body = std::make_shared<PhysicsBody>(shared_from_this());
		m_body->setBoxCollider();
		PhysicsManager::getInstance().addPhysicsObject(m_body->shared_from_this());
	}

	bool GameObject::queryPhysicsEnabled() {
		return m_body != nullptr;
	}

}