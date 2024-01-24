#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

namespace Shard {
    class SHARD_API PhysicsBody;

    class SHARD_API CollisionHandler {
    public:
        virtual void onCollisionEnter(PhysicsBody* body) = 0;
        virtual void onCollisionExit(PhysicsBody* body) = 0;
        virtual void onCollisionStay(PhysicsBody* body) = 0;
    };
}