#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>
#include <vector>

#include "Transform3D.h"

namespace Shard {
    class SHARD_API PhysicsBody;

    class SHARD_API GameObject {
    public:
        Transform3D transform_;
        bool transient_, to_be_destroyed_, visible_;
        PhysicsBody* body_ = nullptr;
        // change this if erases are frequent
        std::vector<std::string> tags {};

        GameObject();

        void addTag(std::string tag);
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
        virtual void killMe() = 0; // needs physicsManager

    };
}