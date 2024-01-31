#pragma once

#include "Display.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <unordered_set>
#include <unordered_map>


namespace Shard {

	typedef unsigned int uint;
	
	class TextDetails {
	public:
		std::string text{};
		double x{};
		double y{};
		SDL_Color col{};
		int size{};
		TTF_Font* font{ nullptr };
		SDL_Texture* lblText{nullptr};

		 TextDetails(const TextDetails& src){
			this->text = src.text;
			this->x = src.x;
			this->y = src.y;
			this->col = src.col;
			this->size = src.size;
			this->font = src.font;
			this->lblText = src.lblText;
		}

		 TextDetails(std::string text, double x, double y, SDL_Color col, int spacing)
		{
			this->text = text;
			this->x = x;
			this->y = y;
			this->col = col;
			this->size = spacing;
		}

		 TextDetails(std::string text, double x, double y, SDL_Color col, int spacing, TTF_Font* font, SDL_Texture* lblText)
		{
			this->text = text;
			this->x = x;
			this->y = y;
			this->col = col;
			this->size = spacing;
			this->font = font;
			this->lblText = lblText;
		}
	};

	class DisplayText : public Display {
	protected:
		SDL_Window* _window{ nullptr };
		SDL_Renderer* _rend{ nullptr };
	public:
		Uint32 _format{};
		int _access{};
		
		 void clearDisplay() override;
		 TTF_Font* loadFont(const std::string& font_name, int size);
		 void display() override;
		 void setFullscreen() override;
		 void initialize() override;
		 void showText(const char* text, double x, double y, int size, int r, int g, int b) override;
        //idk where char[, ] would be used...
        //virtual void showText(char[, ] text, double x, double y, int size, int r, int g, int b);

	private:
		std::vector<TextDetails> myTexts{};
		std::unordered_map<std::string, TTF_Font*> fontLibrary{};
		void update();
		void draw();
	};
}


