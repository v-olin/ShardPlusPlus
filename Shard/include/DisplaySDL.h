// DisplaySDL.h - Description
#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "DisplayText.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Shard {

	class Line {
	public:
		int sx, sy;
		int ex, ey;
		int r, g, b, a;
	};

	class Circle {
	public:
		int x, y, radius;
		int r, g, b, a;
	};

	class DisplaySDL : public DisplayText {
	public:
		SHARD_API DisplaySDL();
		SHARD_API void initialize() override;
		SHARD_API void addToDraw(GameObject gob) override;
		SHARD_API void removeToDraw(GameObject gob) override;
		SHARD_API SDL_Texture* loadTexture(Transform trans);
		SHARD_API SDL_Texture* loadTexture(std::string path);
		SHARD_API void renderCircle(int centreX, int centreY, int radius);
		SHARD_API void drawCircle(int x, int y, int radius, int r, int g, int b, int a) override;
		SHARD_API void drawLine(int x, int y, int x2, int y2, int r, int g, int b, int a) override;
		SHARD_API void display() override;
		SHARD_API void clearDisplay() override;

	private:
		std::vector<Transform> _toDraw;
		std::vector<Line> _linesToDraw;
		std::vector<Circle> _circlesToDraw;
		std::unordered_map<std::string, SDL_Texture*> spriteBuffer;
	};

}