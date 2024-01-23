#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>
#include <glm.hpp>
#include <SDL.h>

#include "PhysicsBody.h"
#include "CollisionHandler.h"
#include "ColliderRect.h"
#include "ColliderCircle.h"

#define NULL_VECTOR glm::vec2{ std::numeric_limits<float>::min(), std::numeric_limits<float>::min()  };

namespace Shard
{
    class SHARD_API Collider { // this is inherited by gameobjects (like asteroid or bullet)
    public:
        Transform* transform;
        float x, y;
        CollisionHandler* game_object;
        glm::vec2 min_and_max_x;
        glm::vec2 min_and_max_y;
        bool rotate_at_offset;

        Collider(CollisionHandler* game_obj, Transform* transform) {
            this->x = 0;
            this->y = 0;
            this->game_object = game_obj;
            this->transform = transform;
        }

        Collider(CollisionHandler* game_obj, Transform* transform, float x, float y) {
            this->x = x;
            this->y = y;
            this->game_object = game_obj;
            this->transform = transform;
        }
        
        virtual void recalculate() = 0;
        virtual glm::vec2 checkCollision(ColliderRect& c) = 0;
        virtual glm::vec2 checkCollision(glm::vec2 c) = 0;
        virtual glm::vec2 checkCollision(ColliderCircle& c) = 0;
        //virtual glm::vec2 checkCollision(Collider c) = 0; // is this neccessary?? (stinky upcast)

        virtual void draw(SDL_Color color) = 0;

    protected:
        bool from_trans;

    };
}
