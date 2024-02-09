#pragma once

#include <memory>
#include <string>
#include <vector>

#include "PhysicsBody.h"

namespace Shard {
    class GameObject : public std::enable_shared_from_this<GameObject> {
    public:
        bool transient_, to_be_destroyed_, visible_;
        std::shared_ptr<PhysicsBody> body_ = nullptr;
        std::vector<const char*> tags{};

        GameObject();
        GameObject(const std::shared_ptr<GameObject> src) {
            body_ = src->body_;
            tags = src->tags;
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