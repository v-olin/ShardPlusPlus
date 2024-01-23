#pragma once


#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>

typedef struct Color {
    int R;
    int G;
    int B;
    int A;
} Color;
//TODO, begone, will not build but no suprise, this is a crazy hack
typedef class Transform {
public:
    std::string spritePath;
    int ht, wid, scalex, scaley, x, y, rotz;
    void recalculateCentre();
} Transform;


typedef struct GameObject {
    Transform transform;
} GameObject;

namespace Shard {
	class SHARD_API Display { // this is inherited by DisplaySDL and DisplayText
	public:
         //protected int _height, _width;

        virtual void drawLine(int x, int y, int x2, int y2, int r, int g, int b, int a);

        virtual void drawLine(int x, int y, int x2, int y2, Color col)
        {
            drawLine(x, y, x2, y2, col.R, col.G, col.B, col.A);
        }


        virtual void drawCircle(int x, int y, int rad, int r, int g, int b, int a);

        virtual void drawCircle(int x, int y, int rad, Color col)
        {
            drawCircle(x, y, rad, col.R, col.G, col.B, col.A);
        }

        virtual void drawFilledCircle(int x, int y, int rad, Color col)
        {
            drawFilledCircle(x, y, rad, col.R, col.G, col.B, col.A);
        }

        virtual void drawFilledCircle(int x, int y, int rad, int r, int g, int b, int a)
        {
            while (rad > 0)
            {
                drawCircle(x, y, rad, r, g, b, a);
                rad -= 1;
            }
        }

        void showText(std::string text, double x, double y, int size, Color col)
        {
            showText(text, x, y, size, col.R, col.G, col.B);
        }



        virtual void setFullscreen();

        virtual void addToDraw(GameObject gob);

        virtual void removeToDraw(GameObject gob);
        int getHeight()
        {
            return _height;
        }

        int getWidth()
        {
            return _width;
        }

        virtual void setSize(int w, int h)
        {
            _height = h;
            _width = w;
        }

        virtual void initialize();
        virtual void clearDisplay();
        virtual void display();

        virtual void showText(std::string text, double x, double y, int size, int r, int g, int b);
        //idk whhere char[, ] would be used...
        //virtual void showText(char[, ] text, double x, double y, int size, int r, int g, int b);
    protected:
        int _height;
        int _width;
	};
}
