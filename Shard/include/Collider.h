#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>
#include <glm.hpp>
#include <SDL.h>
#include <typeinfo>
#include <optional>

#include "CollisionHandler.h"
#include "Transform.h"
// #include "ColliderRect.h"
// #include "ColliderCircle.h"

namespace Shard {
    // forward declarations to stop the compiler from fucking around
    class SHARD_API ColliderCircle;
    class SHARD_API ColliderRect;
    //class SHARD_API CollisionHandler;

    // this is inherited by gameobjects (like asteroid or bullet)
    class SHARD_API Collider {
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
        virtual std::optional<glm::vec2> checkCollision(ColliderRect* other) = 0;
        virtual std::optional<glm::vec2> checkCollision(ColliderCircle* other) = 0;
        virtual std::optional<glm::vec2> checkCollision(glm::vec2 point) = 0;
        
        //std::optional<glm::vec2> checkCollision(Collider* c) {
        //    if (ColliderCircle* circ_c = dynamic_cast<ColliderCircle*>(c)) {
        //        if(ColliderCircle* circ_this = dynamic_cast<ColliderCircle*>(this)) {
        //            return circ_this->checkCollision(circ_c);
        //        } else {
        //            ColliderRect* rect_this = dynamic_cast<ColliderRect*>(this);
        //            return rect_this->checkCollision(circ_c);
        //        }
        //    } else if (ColliderRect* rect_c = dynamic_cast<ColliderRect*>(c)) {
		//		if (ColliderCircle* circ_this = dynamic_cast<ColliderCircle*>(this)) {
		//			return circ_this->checkCollision(rect_c);
		//		}
		//		else {
		//			ColliderRect* rect_this = dynamic_cast<ColliderRect*>(this);
		//			return rect_this->checkCollision(rect_c);
		//		}
        //    }
        //
        //    //if (typeid(c) == typeid(ColliderCircle())) {
        //    //} else {
        //    //    ColliderRect* rect = static_cast<ColliderRect*>(c);
        //    //    return checkCollision(rect);
        //    //}
        //    return std::nullopt;
        //}

        virtual void draw(SDL_Color color) = 0;

    protected:
        bool from_trans;

    };

    class SHARD_API ColliderCircle : public Collider {
    public:
        float rad;
        float x_off, y_off;

        //ColliderCircle();
        ColliderCircle(CollisionHandler* game_obj, Transform* transform);
        ColliderCircle(CollisionHandler* game_obj, Transform* transform, float x, float y, float rad);

        void calculateBoundingBox();

        // inherited from Collider
        void recalculate() override;
        std::optional<glm::vec2> checkCollision(ColliderRect* other) override;
        std::optional<glm::vec2> checkCollision(ColliderCircle* other) override;
        std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
        void draw(SDL_Color color) override;
    };

    class SHARD_API ColliderRect : public Collider {
    public:
        float width, height, base_width, base_height;
        float left, right, top, bottom;

        ColliderRect();
        ColliderRect(CollisionHandler* game_obj, Transform* transform);
        ColliderRect(CollisionHandler* game_obj, Transform* transform, float x, float y, float width, float height);

        void calculateBoundingBox();
        ColliderRect calculateMinkowskiDifference(ColliderRect& other);
        glm::vec2 calculatePenetration(glm::vec2 point);

        // inherited from Collider
        void recalculate() override;
        std::optional<glm::vec2> checkCollision(ColliderRect* other) override;
        std::optional<glm::vec2> checkCollision(ColliderCircle* other) override;
        std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
        void draw(SDL_Color color) override;


    private:
        bool for_minkowski = false;

    };
}
