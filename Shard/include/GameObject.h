#pragma once

#include <string>
#include <vector>

#include "Transform3D.h"
#include "PhysicsBody.h"

namespace Shard {
    //class  PhysicsBody;

    class  GameObject {
    public:
        Transform3D* transform_;
        bool transient_, to_be_destroyed_, visible_;
        PhysicsBody* body_ = nullptr;
        // change this if erases are frequent
        std::vector<const char*> tags{};

        GameObject();
        GameObject(const GameObject* src) {
            transform_ = src->transform_;
            body_ = src->body_;
            tags = src->tags;
        }
        ~GameObject() {}

        void addTag(const char* tag);
        void removeTag(std::string tag);
        bool hasTag(std::string tag);
        std::string getTags();
        void setPhysicsEnabled();
        bool queryPhysicsEnabled();
        void checkDestroyMe();

        // to be inherited by other objects
        virtual void initialize() = 0;
        virtual void update() = 0;
        virtual void physicsUpdate() = 0;
        virtual void prePhysicsUpdate() = 0;
        // needs physicsManager
        virtual void killMe() = 0;
    };
}