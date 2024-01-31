#pragma once

#include <string>
//#include <glm.hpp>
#include <SDL.h>
#include <typeinfo>
#include <optional>

#include "CollisionHandler.h"
#include "Transform.h"

namespace Shard {
    class  ColliderCircle;
    class  ColliderRect;

    class  Collider {
    public:
        float x, y;
        glm::vec2 box_top_left{};
        glm::vec2 box_bottom_right{};
        glm::vec2 min_and_max_x;
        glm::vec2 min_and_max_y;

        glm::vec2 top_left;
        glm::vec2 bottom_right;

        bool rotate_at_offset;
        Transform* transform;
        CollisionHandler* game_object;

        Collider()
            : x(0), y(0),
            rotate_at_offset(false),
            transform(nullptr),
            game_object(nullptr) { }

        Collider(CollisionHandler* game_obj, Transform* transform)
            : x(0), y(0),
            rotate_at_offset(false),
            transform(nullptr),
            game_object(nullptr)
        {
            this->x = 0;
            this->y = 0;
            this->game_object = game_obj;
            this->transform = transform;
        }

        Collider(CollisionHandler* game_obj, Transform* transform, float x, float y)
            : x(0), y(0),
            rotate_at_offset(false),
            transform(nullptr),
            game_object(nullptr)
        {
            this->x = x;
            this->y = y;
            this->game_object = game_obj;
            this->transform = transform;
        }

        virtual void recalculate() = 0;
        virtual std::optional<glm::vec2> checkCollision(glm::vec2 point) = 0;
        virtual std::optional<glm::vec2> checkCollision(Collider* c) = 0;
        virtual void draw(SDL_Color color) = 0;

    protected:
        bool from_trans{false};

    };

    class  ColliderCircle : public Collider {
    public:
        float rad;
        float x_off, y_off;

        ColliderCircle();
        ColliderCircle(CollisionHandler* game_obj, Transform* transform);
        ColliderCircle(CollisionHandler* game_obj, Transform* transform, float x, float y, float rad);

        void calculateBoundingBox();

        // inherited from Collider
        void recalculate() override;
        std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
        std::optional<glm::vec2> checkCollision(Collider* c) override;
        void draw(SDL_Color color) override;

        //internal colliding check
        std::optional<glm::vec2> checkCollision(ColliderRect* other) ;
        std::optional<glm::vec2> checkCollision(ColliderCircle* other);
    };

    class  ColliderRect : public Collider {
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
        std::optional<glm::vec2> checkCollision(glm::vec2 point) override;
        std::optional<glm::vec2> checkCollision(Collider* c) override;
        void draw(SDL_Color color) override;

        //internal colliding check
        std::optional<glm::vec2> checkCollision(ColliderRect* other) ;
        std::optional<glm::vec2> checkCollision(ColliderCircle* other);

    private:
        bool for_minkowski = false;

    };
}
