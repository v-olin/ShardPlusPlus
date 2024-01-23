#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>

#include "GameObject.h"

namespace Shard {
    class SHARD_API Transform {
    public:
        GameObject* owner;
        float x, y, lx, ly, rotz, scale_x, scale_y;
        int w, h;
        std::string sprite_path;
        glm::vec2 forward, right, centre;

        Transform();
        Transform(GameObject* owner);

        void recalculateCentre();
        void translate(float x, float y);
        void translate(glm::vec2 vect);
        void rotate(float dir);
        glm::vec2 getLastDirection();
    };
}