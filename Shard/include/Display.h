#pragma once


#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "Transform.h"
#include "GameObject.h"
#include <string>

namespace Shard {

    typedef struct Color {
        int R;
        int G;
        int B;
        int A;
    } Color;
	
    class  Display { // this is inherited by DisplaySDL and DisplayText
	public:

        virtual SHARD_API void drawLine(int x, int y, int x2, int y2, int r, int g, int b, int a);

        virtual SHARD_API void drawLine(int x, int y, int x2, int y2, Color col)
        {
            drawLine(x, y, x2, y2, col.R, col.G, col.B, col.A);
        }


        virtual SHARD_API void drawCircle(int x, int y, int rad, int r, int g, int b, int a);

        virtual SHARD_API void drawCircle(int x, int y, int rad, Color col)
        {
            drawCircle(x, y, rad, col.R, col.G, col.B, col.A);
        }

        virtual SHARD_API void drawFilledCircle(int x, int y, int rad, Color col)
        {
            drawFilledCircle(x, y, rad, col.R, col.G, col.B, col.A);
        }

        virtual SHARD_API void drawFilledCircle(int x, int y, int rad, int r, int g, int b, int a)
        {
            while (rad > 0)
            {
                drawCircle(x, y, rad, r, g, b, a);
                rad -= 1;
            }
        }

        SHARD_API void showText(std::string text, double x, double y, int size, Color col)
        {
            showText(text, x, y, size, col.R, col.G, col.B);
        }



        virtual SHARD_API void setFullscreen();

        virtual SHARD_API void addToDraw(GameObject gob);

        virtual SHARD_API void removeToDraw(GameObject gob);
        SHARD_API int getHeight()
        {
            return _height;
        }

        SHARD_API int getWidth()
        {
            return _width;
        }

        virtual SHARD_API void setSize(int w, int h)
        {
            _height = h;
            _width = w;
        }

        virtual SHARD_API void initialize();
        virtual SHARD_API void clearDisplay();
        virtual SHARD_API void display();

        virtual SHARD_API void showText(std::string text, double x, double y, int size, int r, int g, int b);
        //idk whhere char[, ] would be used...
        //virtual void showText(char[, ] text, double x, double y, int size, int r, int g, int b);
    protected:
        int _height;
        int _width;
	};
}
