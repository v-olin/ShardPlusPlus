#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <glm.hpp>
#include <SDL.h>

#include "PhysicsBody.h"

namespace Shard
{
    class SHARD_API Transform {

    };

    class SHARD_API GameObject : CollisionHandler {

    };

    class SHARD_API CollisionHandler {
    public:
        virtual void onCollisionEnter(PhysicsBody body) = 0;
        virtual void onCollisionExit(PhysicsBody body) = 0;
        virtual void onCollisionStay(PhysicsBody body) = 0;

    };

    class SHARD_API ColliderRect : Collider {
    public:
        Transform rect;
        float x, y, width, height, base_width, base_height;
        float left, right, top, bottom;

        ColliderRect(CollisionHandler game_obj, Transform transform);
        ColliderRect(CollisionHandler game_obj, Transform transform, float x, float y, float width, float height);

        void calculateBoundingBox();
        void recalculate();
        void draw(SDL_Color color);
        ColliderRect calculateMinkowskiDifference(ColliderRect other);
        glm::vec2 calculatePenetration(ColliderRect other);
        glm::vec2 checkCollision(ColliderRect other);
        glm::vec2 checkCollision(ColliderCircle other);
        glm::vec2 checkCollision(glm::vec2 other);


    private:
        bool fromTrans;

        class ColliderRect : Collider
        {
            /*
            public ColliderRect(CollisionHandler gob, Transform t) : base(gob)
            {

                this.MyRect = t;
                fromTrans = true;
                RotateAtOffset = false;
                calculateBoundingBox();
            }

            public ColliderRect(CollisionHandler gob, Transform t, float x, float y, float wid, float ht) : base(gob)
            {
                X = x;
                Y = y;
                BaseWid = wid;
                BaseHt = ht;
                RotateAtOffset = true;

                this.MyRect = t;

                fromTrans = false;

            }

            public void calculateBoundingBox()
            {
                float nwid, nht, angle, x1, x2, y1, y2;
                double cos, sin;
                if (myRect == null)
                {
                    return;
                }

                if (fromTrans)
                {
                    Wid = (float)(MyRect.Wid * MyRect.Scalex);
                    Ht = (float)(MyRect.Ht * MyRect.Scaley);
                }
                else
                {
                    Wid = (float)(BaseWid * MyRect.Scalex);
                    Ht = (float)(BaseHt * MyRect.Scaley);
                }

                angle = (float)(Math.PI * MyRect.Rotz / 180.0f);


                cos = Math.Cos(angle);
                sin = Math.Sin(angle);

                // Bit of trig here to calculate the new height and width
                nwid = (float)(Math.Abs(Wid * cos) + Math.Abs(Ht * sin));
                nht = (float)(Math.Abs(Wid * sin) + Math.Abs(Ht * cos));

                X = (float)MyRect.X + (Wid / 2);
                Y = (float)MyRect.Y + (Ht / 2);

                Wid = nwid;
                Ht = nht;

                if (RotateAtOffset) {
                    // Now we work out the X and Y based on the rotation of the body to 
                    // which this belongs,.
                    x1 = X - MyRect.Centre.X;
                    y1 = Y - MyRect.Centre.Y;

                    x2 = (float)(x1 * Math.Cos(angle) - y1 * Math.Sin(angle));
                    y2 = (float)(x1 * Math.Sin(angle) + y1 * Math.Cos(angle));

                    X = x2 + (float)MyRect.Centre.X;
                    Y = y2 + (float)MyRect.Centre.Y;
                }

                MinAndMaxX[0] = X - Wid / 2; // left
                MinAndMaxX[1] = X + Wid / 2; // right
                MinAndMaxY[0] = Y - Ht / 2; // top
                MinAndMaxY[1] = Y + Ht / 2; // bottom


            }
            */


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

    };

    class SHARD_API ColliderCircle {

    };

    class SHARD_API Collider { // this is inherited by gameobjects (like asteroid or bullet)
    public:
        Collider(CollisionHandler game_obj);

        CollisionHandler gameObject;
        glm::vec2 min_and_max_x;
        glm::vec2 min_and_max_y;
        bool rotate_at_offset;
        
        virtual void recalculate() = 0;
        virtual glm::vec2 checkCollision(ColliderRect c) = 0;
        virtual glm::vec2 checkCollision(glm::vec2 c) = 0;
        virtual glm::vec2 checkCollision(ColliderCircle c) = 0;
        virtual glm::vec2 checkCollision(Collider c) = 0; // is this neccessary?? (stinky downcast)

        virtual void draw(SDL_Color color) = 0;
        virtual glm::vec2 getMinAndMaxX() = 0;
        virtual glm::vec2 getMinAndMaxY() = 0;
    };
}
