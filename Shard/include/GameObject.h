#pragma once

#include <memory>
#include <string>
#include <vector>

#include "PhysicsBody.h"
#include "Model.h"

namespace Shard {
    class GameObject : public std::enable_shared_from_this<GameObject> {
    public:
        bool m_transient, m_toBeDestroyed, m_visible;
        std::shared_ptr<PhysicsBody> m_body;
        std::shared_ptr<Model> m_model;
        std::vector<const char*> m_tags;

        GameObject();

        GameObject(const std::shared_ptr<GameObject> src) {
            m_body = src->m_body;
            m_tags = src->m_tags;
        }

        virtual ~GameObject() {}

        void addTag(const char* tag);
        void removeTag(std::string tag);
        bool hasTag(std::string tag);
        std::string getTags();
        void setPhysicsEnabled();
        bool queryPhysicsEnabled();

        // to be inherited by other objects
        virtual void checkDestroyMe() = 0;
        virtual void initialize() = 0;
        virtual void update() = 0;
        virtual void physicsUpdate() = 0;
        virtual void prePhysicsUpdate() = 0;
        // needs physicsManager
        virtual void killMe() = 0;
    };
}