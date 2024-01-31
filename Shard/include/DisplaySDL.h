// DisplaySDL.h - Description
#pragma once

#include "DisplayText.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Shard {

	class Line {
	public:
		int sx, sy;
		int ex, ey;
		int r, g, b, a{255};
	};

	class Circle {
	public:
		int x, y, radius;
		int r, g, b, a{255};
	};

	class DisplaySDL : public DisplayText {
	public:
		 DisplaySDL() {}
		 void initialize() override;
		 void addToDraw(GameObject* gob) override;
		 void removeToDraw(GameObject* gob) override;
		 SDL_Texture* loadTexture(Transform* trans);
		 SDL_Texture* loadTexture(std::string path);
		 void renderCircle(int centreX, int centreY, int radius);
		 void drawCircle(int x, int y, int radius, int r, int g, int b, int a) override;
		 void drawLine(int x, int y, int x2, int y2, int r, int g, int b, int a) override;
		 void display() override;
		 void clearDisplay() override;

	private:
		std::vector<Transform*> _toDraw;
		std::vector<Line> _linesToDraw;
		std::vector<Circle> _circlesToDraw;
		std::unordered_map<std::string, SDL_Texture*> spriteBuffer;
	};

}