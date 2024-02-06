#include "DisplayText.h"
#include "Logger.h"

#include <filesystem>

namespace Shard {

	void DisplayText::clearDisplay() {
		for (const TextDetails &td : myTexts)
			SDL_DestroyTexture(td.lblText);

		myTexts.clear();
		// Reset clear-color to black
		SDL_SetRenderDrawColor(_rend, 0, 0, 0, 255);
		SDL_RenderClear(_rend);
	}

	TTF_Font* DisplayText::loadFont(const std::string& font_name, int size) {

		if (fontLibrary.find(font_name) != fontLibrary.end())
			return fontLibrary.at(font_name);

		std::filesystem::path path_to_font = std::filesystem::current_path();
		path_to_font.append("assets");
		path_to_font.append("fonts");
		path_to_font.append(font_name);

		auto font = TTF_OpenFont(path_to_font.string().c_str(), size);
		if (font == NULL) {
			Logger::log("DisplayText.cpp loadFont: Could not open font " + font_name, LOG_LEVEL_ALL);
			return nullptr;
		}

		fontLibrary[font_name] = font;
		return fontLibrary.at(font_name);
	}
	
	void DisplayText::display() {
		update();
		draw();
	}
	
	void DisplayText::setFullscreen() {
		SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	void DisplayText::initialize() {
		Logger::log("Inintialized Display");
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
		Logger::log("Inintialized Display");
	}

	void DisplayText::showText(const char* text, double x, double y, int size, int r, int g, int b) {
		int nx, ny, w = 0, h = 0;
		SDL_Color col{};
		col.r = (Uint8)r;
		col.g = (Uint8)g;
		col.b = (Uint8)b;
		col.a = (Uint8)255;
		TTF_Font* font = loadFont("calibri.ttf", size);

		auto td = TextDetails(text, x, y, col, 12);
		td.font = font;

		SDL_Surface* surf = TTF_RenderText_Blended(td.font, td.text.data(), td.col);
		SDL_Texture* lblText = SDL_CreateTextureFromSurface(_rend, surf);
		if (!lblText) {
			Logger::log("[error in DisplayText::showText] texture fucky wucky");
			// Handle texture creation error
		}
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
		for (const TextDetails& td : myTexts) {
			SDL_Rect sRect{};

			sRect.x = (int)td.x;
			sRect.y = (int)td.y;
			sRect.w = 0;
			sRect.h = 0;

			TTF_SizeText(td.font, td.text.c_str(), &sRect.w, &sRect.h);
			SDL_RenderCopy(_rend, td.lblText, NULL, &sRect);
		}
		// FUCK SDL!!
		SDL_RenderPresent(_rend);
	}

}