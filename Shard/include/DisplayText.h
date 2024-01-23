#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "Display.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <unordered_set>
#include <unordered_map>


namespace Shard {

	typedef unsigned int uint;
	
	class SHARD_API TextDetails {
	public:
		std::string text;
		double x;
		double y;
		SDL_Color col;
		int size;
		TTF_Font* font;
		SDL_Texture* lblText;

		TextDetails(std::string text, double x, double y, SDL_Color col, int spacing)
		{
			this->text = text;
			this->x = x;
			this->y = y;
			this->col = col;
			this->size = spacing;
		}
	};

	class SHARD_API DisplayText : public Display {
	protected:
		SDL_Window* _window;
		SDL_Renderer* _rend;
	public:
		Uint32 _format;
		int _access;
		
		void clearDisplay() override;
		TTF_Font* loadFont(std::string path, int size);
		void display() override;
		void setFullscreen() override;
		void initialize() override;
		void showText(std::string text, double x, double y, int size, int r, int g, int b) override;
        //idk where char[, ] would be used...
        //virtual void showText(char[, ] text, double x, double y, int size, int r, int g, int b);

	private:
		std::unordered_set<TextDetails> myTexts{};
		std::unordered_map<std::string, TTF_Font*> fontLibrary{};
		void update();
		void draw();







	};
}


