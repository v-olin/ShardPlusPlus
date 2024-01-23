#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "Collider.h"
#include "CollisionHandler.h"
#include "ColliderCircle.h"
#include "Transform.h"

namespace Shard {
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
        void recalculate();
        glm::vec2 checkCollision(ColliderRect& other) override;
        glm::vec2 checkCollision(ColliderCircle& other) override;
        glm::vec2 checkCollision(glm::vec2 point) override;
        void draw(SDL_Color color) override;


    private:
        bool for_minkowski = false;

        /*
        class ColliderRect : Collider
        {

            public ColliderRect calculateMinkowskiDifference(ColliderRect other)
            {
                float left, right, top, bottom, width, height;
                ColliderRect mink = new ColliderRect(null, null);

                // A set of calculations that gives us the Minkowski difference
                // for this intersection.
                left = Left - other.Right;
                top = other.Top - Bottom;
                width = Wid + other.Wid;
                height = Ht + other.Ht;
                right = Right - other.Left;
                bottom = other.Bottom - Top;

                mink.Wid = width;
                mink.Ht = height;

                mink.MinAndMaxX = new float[2] { left, right };
                mink.MinAndMaxY = new float[2] { top, bottom };

                return mink;
            }

            public Vector2 ? calculatePenetration(Vector2 checkPoint)
            {
                Vector2 ? impulse;
                float coff = 0.2f;

                // Check the right edge
                float min;

                min = Math.Abs(Right - checkPoint.X);
                impulse = new Vector2(-1 * min - coff, checkPoint.Y);


                // Now compare against the Left edge
                if (Math.Abs(checkPoint.X - Left) <= min)
                {
                    min = Math.Abs(checkPoint.X - Left);
                    impulse = new Vector2(min + coff, checkPoint.Y);
                }

                // Now the bottom
                if (Math.Abs(Bottom - checkPoint.Y) <= min)
                {
                    min = Math.Abs(Bottom - checkPoint.Y);
                    impulse = new Vector2(checkPoint.X, min + coff);
                }

                // And now the top
                if (Math.Abs(Top - checkPoint.Y) <= min)
                {
                    min = Math.Abs(Top - checkPoint.Y);
                    impulse = new Vector2(checkPoint.X, -1 * min - coff);
                }

                return impulse;
            }

            public override Vector2 ? checkCollision(ColliderRect other)
            {
                ColliderRect cr;

                cr = calculateMinkowskiDifference(other);

                if (cr.Left <= 0 && cr.Right >= 0 && cr.Top <= 0 && cr.Bottom >= 0)
                {
                    return cr.calculatePenetration(new Vector2(0, 0));
                }



                return null;

            }

            public override void drawMe(Color col)
            {
                Display d = Bootstrap.getDisplay();

                d.drawLine((int)MinAndMaxX[0], (int)MinAndMaxY[0], (int)MinAndMaxX[1], (int)MinAndMaxY[0], col);
                d.drawLine((int)MinAndMaxX[0], (int)MinAndMaxY[0], (int)MinAndMaxX[0], (int)MinAndMaxY[1], col);
                d.drawLine((int)MinAndMaxX[1], (int)MinAndMaxY[0], (int)MinAndMaxX[1], (int)MinAndMaxY[1], col);
                d.drawLine((int)MinAndMaxX[0], (int)MinAndMaxY[1], (int)MinAndMaxX[1], (int)MinAndMaxY[1], col);

                d.drawCircle((int)X, (int)Y, 2, col);
            }

            public override Vector2 ? checkCollision(ColliderCircle c)
            {
                Vector2 ? possibleV = c.checkCollision(this);

                if (possibleV is Vector2 v)
                {
                    v.X *= -1;
                    v.Y *= -1;
                    return v;
                }

                return null;
            }

            public override Vector2 ? checkCollision(Vector2 other)
            {

                if (other.X >= Left &&
                    other.X <= Right &&
                    other.Y >= Top &&
                    other.Y <= Bottom)
                {
                    return new Vector2(0, 0);
                }

                return null;
            }

        }

        */
    };
}