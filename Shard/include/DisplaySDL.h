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

	class SHARD_API Line {
	public:
		int sx, sy;
		int ex, ey;
		int r, g, b, a;
	};

	class SHARD_API Circle {
	public:
		int x, y, radius;
		int r, g, b, a;
	};

	class SHARD_API DisplaySDL : public DisplayText {
	public:
		void initialize() override;
		
		void addToDraw(GameObject gob) override;
		void removeToDraw(GameObject gob) override;

		SDL_Texture* loadTexture(Transform trans);
		SDL_Texture* loadTexture(std::string path);

		void renderCircle(int centreX, int centreY, int radius);
		
		void drawCircle(int x, int y, int radius, int r, int g, int b, int a) override;
		void drawLine(int x, int y, int x2, int y2, int r, int g, int b, int a) override;

		void display() override;
		void clearDisplay() override;

	private:
		std::unordered_set<Transform> _toDraw;
		std::unordered_set<Line> _linesToDraw;
		std::unordered_set<Circle> _circlesToDraw;
		// TODO: replace 'int' with proper type
		std::unordered_map<std::string, SDL_Texture*> spriteBuffer;
	};

}