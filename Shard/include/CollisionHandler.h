#pragma once

namespace Shard {
    class  PhysicsBody;

    class  CollisionHandler {
    public:
        virtual void onCollisionEnter(std::shared_ptr<PhysicsBody> body) = 0;
        virtual void  onCollisionExit(std::shared_ptr<PhysicsBody> body) = 0;
        virtual void  onCollisionStay(std::shared_ptr<PhysicsBody> body) = 0;
    };
}