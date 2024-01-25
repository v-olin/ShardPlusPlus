#include "DisplayText.h"
#include "Logger.h"

namespace Shard {

	void DisplayText::clearDisplay() {
		for (const TextDetails &td : myTexts)
			SDL_DestroyTexture(td.lblText);
		myTexts.clear();
		SDL_RenderClear(_rend);
	}

	TTF_Font* DisplayText::loadFont(std::string path, int size) {
		std::string key = path + "," + std::to_string(size);
		if (fontLibrary.find(key) != fontLibrary.end())
			return fontLibrary.at(key);
		
		auto font = TTF_OpenFont(path.data(), size);

		if (font == NULL) {
			Logger::log("DisplayText.cpp loadFont: Could not open font " + path, LOG_LEVEL_ALL);
			return nullptr;
		}

		fontLibrary.at(key) = font;

		return fontLibrary.at(key);
	}
	
	void DisplayText::display() {
		update();
		draw();
	}
	
	void DisplayText::setFullscreen() {
		SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	void DisplayText::initialize() {
		setSize(1280, 864);
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();
		_window = SDL_CreateWindow("ShardPlusPlus Game Engine",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			getWidth(),
			getHeight(),
			0);
		_rend = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
		SDL_SetRenderDrawBlendMode(_rend, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(_rend, 0, 0, 0, 225);
	}

	void DisplayText::showText(std::string text, double x, double y, int size, int r, int g, int b) {
		int nx, ny, w = 0, h = 0;
		SDL_Color col{};
		col.r = (Uint8)r;
		col.g = (Uint8)g;
		col.b = (Uint8)b;
		col.a = (Uint8)255;
		TTF_Font* font = loadFont("Fonts/calibri.ttf", size);

		
		TextDetails td = TextDetails(text, x, y, col, 12);
		td.font = font;

		SDL_Surface* surf = TTF_RenderText_Blended(td.font, td.text.data(), td.col);
		SDL_Texture* lblText = SDL_CreateTextureFromSurface(_rend, surf);
		SDL_FreeSurface(surf);

		SDL_Rect sRect{};
		
		sRect.x = (int)x;
		sRect.y = (int)y;
		sRect.w = w;
		sRect.h = h;

		SDL_QueryTexture(lblText, &_format, &_access, &sRect.w, &sRect.h);
		td.lblText = lblText;

		myTexts.push_back(td);
	}

	void DisplayText::update() {

	}

	void DisplayText::draw() {
		for (const TextDetails td : myTexts) {
			SDL_Rect sRect{};

			sRect.x = (int)td.x;
			sRect.y = (int)td.y;
			sRect.w = 0;
			sRect.h = 0;

			TTF_SizeText(td.font, td.text.data(), &sRect.w, &sRect.h);
			SDL_RenderCopy(_rend, td.lblText, NULL, &sRect);
		}
		SDL_RenderPresent(_rend);
	}

}