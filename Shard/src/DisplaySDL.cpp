#include "DisplaySDL.h"

#include <SDL.h>
#include <SDL_image.h>

namespace Shard {

	void DisplaySDL::initialize() {
		DisplayText::initialize();
	}

	// TODO: replace return type 'int' with proper type
	SDL_Texture* DisplaySDL::loadTexture(Transform trans) {
		SDL_Texture* ret;
		unsigned int format;
		int access;
		int w, h;

		ret = loadTexture(trans.spritePath);
		SDL_QueryTexture(ret, &format, &access, &w, &h);

		trans.ht = h;
		trans.wid = w;
		//TODO: 
		//trans.recalculateCentre();

		// TODO: cursed cast that will create a black hole?
		return ret;
	}

	// TODO: replace return type 'int' with proper type
	SDL_Texture* DisplaySDL::loadTexture(std::string path) {
		SDL_Surface* img;

		//if (spriteBuffer.find(path) != spriteBuffer.end())
		if (spriteBuffer.find(0) != spriteBuffer.end())
			// TODO, this may very well fuck us later
			return spriteBuffer[path];

		img = IMG_Load(path.data());
	
		// TODO: Uncomment once Debug module is available
		// Debug.getInstance().log("IMG_Load: " + IMG_GetError());

		spriteBuffer[path] = SDL_CreateTextureFromSurface(_rend, img);
		SDL_SetTextureBlendMode(spriteBuffer[path], SDL_BLENDMODE_BLEND);

		//TODO, return was img in C#, check if this change works
		return spriteBuffer[path];
	}

	void DisplaySDL::addToDraw(GameObject gob) {
		_toDraw.push_back(gob.transform);
		if (gob.transform.spritePath.empty())
			return;
		loadTexture(gob.transform.spritePath);
	}

	void DisplaySDL::removeToDraw(GameObject gob) {
		auto iter = _toDraw.begin();

		while (++iter != _toDraw.end()) {
			if (gob.transform.ht == (*iter).ht)
				_toDraw.erase(iter);
			return;
		}
	}

	void DisplaySDL::renderCircle(int centreX, int centreY, int radius) {
		int dia = (radius * 2);
		Uint8 r, g, b, a;		
		int x = (radius - 1);
		int y = 0;
		int tx = 1;
		int ty = 1;
		int error = (tx - dia);

		SDL_GetRenderDrawColor(_rend, &r, &g, &b, &a);

		while (x >= y) {

			SDL_RenderDrawPoint(_rend, centreX + x, centreY - y);
			SDL_RenderDrawPoint(_rend, centreX + x, centreY + y);
			SDL_RenderDrawPoint(_rend, centreX - x, centreY - y);
			SDL_RenderDrawPoint(_rend, centreX - x, centreY + y);
			SDL_RenderDrawPoint(_rend, centreX + y, centreY - x);
			SDL_RenderDrawPoint(_rend, centreX + y, centreY + x);
			SDL_RenderDrawPoint(_rend, centreX - y, centreY - x);
			SDL_RenderDrawPoint(_rend, centreX - y, centreY + x);

			if (error <= 0) {
				y += 1;
				error += ty;
				ty += 2;
			}

			if (error > 0) {
				x -= 1;
				tx += 2;
				error += (tx - dia);
			}

		}
	}

	void DisplaySDL::drawCircle(int x, int y, int rad, int r, int g, int b, int a) {
		Circle c;

		c.x = x;
		c.y = y;
		c.radius = rad;

		c.r = r;
		c.g = g;
		c.b = b;
		c.a = a;

		_circlesToDraw.push_back(c);

		
	}

	void DisplaySDL::drawLine(int x, int y, int x2, int y2, int r, int g, int b, int a) {
		Line l;
		l.sx = x;
		l.sy = y;
		l.ex = x2;
		l.ey = y2;

		l.r = r;
		l.g = g;
		l.b = b;
		l.a = a;

		_linesToDraw.push_back(l);
	}

	void DisplaySDL::display() {
		SDL_Rect sRect;
		SDL_Rect tRect;

		for (const Transform trans : _toDraw) {
			//if (trans.spritePath.empty())
				//continue;
			SDL_Texture* sprite = loadTexture(trans);

			sRect.x = 0;
			sRect.y = 0;
			sRect.w = (int)(trans.wid * trans.scalex);
			sRect.h = (int)(trans.ht * trans.scaley);

			tRect.x = (int)trans.x;
			tRect.y = (int)trans.y;
			tRect.w = sRect.w;
			tRect.h = sRect.h;

			SDL_RenderCopyEx(_rend, sprite, &sRect, &tRect, trans.rotz, NULL, SDL_FLIP_NONE);
			
		}
		
		for (const Circle c : _circlesToDraw) {
			SDL_SetRenderDrawColor(_rend, c.r, c.g, c.b, c.a);
			renderCircle(c.x, c.y, c.radius);
		}

		for (const Line l : _linesToDraw) {
			SDL_SetRenderDrawColor(_rend, (Uint8)l.r, (Uint8)l.g, (Uint8)l.b, (Uint8)l.a);
			SDL_RenderDrawLine(_rend, l.sx, l.sy, l.ex, l.ey);
		}

		DisplayText::display();
	}

	void DisplaySDL::clearDisplay() {
		_toDraw.clear();
		_circlesToDraw.clear();
		_linesToDraw.clear();
		DisplayText::clearDisplay();
	}

}