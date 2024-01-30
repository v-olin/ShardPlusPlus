#pragma once

namespace Shard {
    class  PhysicsBody;

    class  CollisionHandler {
    public:
        virtual void onCollisionEnter(PhysicsBody* body) = 0;
        virtual void onCollisionExit(PhysicsBody* body) = 0;
        virtual void onCollisionStay(PhysicsBody* body) = 0;
    };
}