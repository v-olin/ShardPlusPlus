#pragma once

#include <memory>
#include <string>
#include <glm.hpp>

namespace Shard {
    class Transform : public std::enable_shared_from_this<Transform> {
    public:
        float x{}, y{}, lx{}, ly{}, rotz{}, scale_x{}, scale_y{};
        int w{}, h{};
        //const char* sprite_path;
        std::string sprite_path;
        glm::vec2 forward, right, centre;

        Transform();
        Transform(std::shared_ptr<Transform> src) {
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
            if (!src->sprite_path.empty()) {
                sprite_path = src->sprite_path;
                /*
                // Allocate memory for the new string
                sprite_path = new char[std::strlen(src->sprite_path) + 1];

                // Copy the content of the source string to the new string
                std::strcpy(const_cast<char*>(sprite_path), src->sprite_path);
				*/
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