#pragma once

#include "Transform.h"
#include "GameObject.h"
#include <string>
#include <SDL.h>

namespace Shard {

	
    class  Display { // this is inherited by DisplaySDL and DisplayText
	public:
        virtual void drawLine(int x, int y, int x2, int y2, int r, int g, int b, int a) {}
        virtual void drawLine(int x, int y, int x2, int y2, SDL_Color col) {
            drawLine(x, y, x2, y2, col.r, col.g, col.b, col.a);
        }

        virtual void drawCircle(int x, int y, int rad, int r, int g, int b, int a) {}

        virtual void drawCircle(int x, int y, int rad, SDL_Color col) {
            drawCircle(x, y, rad, col.r, col.g, col.b, col.a);
        }

        virtual void drawFilledCircle(int x, int y, int rad, SDL_Color col) {
            drawFilledCircle(x, y, rad, col.r, col.g, col.b, col.a);
        }

        virtual  void drawFilledCircle(int x, int y, int rad, int r, int g, int b, int a) {
            while (rad > 0) {
                drawCircle(x, y, rad, r, g, b, a);
                rad -= 1;
            }
        }

        void showText(const char* text, double x, double y, int size, SDL_Color col) {
            showText(text, x, y, size, col.r, col.g, col.b);
        }
        
        int getHeight() {
            return _height;
        }

        int getWidth() {
            return _width;
        }

        virtual void setSize(int w, int h) {
            _height = h;
            _width = w;
        }

        virtual void setFullscreen() = 0;
        virtual void addToDraw(std::shared_ptr<GameObject> gob) = 0;
        virtual void removeToDraw(std::shared_ptr<GameObject> gob) = 0;
        virtual void initialize() = 0;
        virtual void clearDisplay() = 0;
        virtual void display() = 0;
        virtual void showText(const char* text, double x, double y, int size, int r, int g, int b) {}
    
    protected:
        int _height;
        int _width;
	};
}
