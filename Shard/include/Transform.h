#pragma once

#include <string>
#include <glm.hpp>

namespace Shard {
    class  Transform {
    public:
        float x{}, y{}, lx{}, ly{}, rotz{}, scale_x{}, scale_y{};
        int w{}, h{};
        const char* sprite_path;
        glm::vec2 forward, right, centre;

        Transform();

        Transform(const Transform* src) {
            // Copy primitive types directly
            x = src->x;
            y = src->y;
            lx = src->lx;
            ly = src->ly;
            rotz = src->rotz;
            scale_x = src->scale_x;
            scale_y = src->scale_y;
            w = src->w;
            h = src->h;

            // Deep copy for sprite_path
            if (src->sprite_path) {
                // Allocate memory for the new string
                sprite_path = new char[std::strlen(src->sprite_path) + 1];

                // Copy the content of the source string to the new string
                std::strcpy(const_cast<char*>(sprite_path), src->sprite_path);
            }
            else {
                sprite_path = nullptr;  // If source is null, set the target to null
            }

            // Copy glm::vec2 members
            forward = src->forward;
            right = src->right;
            centre = src->centre;
        }

	    
        void recalculateCentre();
        void translate(float x, float y);
        void translate(glm::vec2 vect);
        void rotate(float dir);
        glm::vec2 getLastDirection();
    };
}