#pragma once

#include "../AST.h"
#include "../PerlinNoise.h"
#include <string>

class Scene_Title : public AST::Scene {
	AST::Rect Background;
	public:
		Scene_Title();
		void loop() override;
		~Scene_Title();
};


class Scene_Game : public AST::Scene {

	PerlinNoise pn;
	AST::Rect Background;
	std::vector<AST::Rect> tiles;
	SDL_Point cords;

	void scroll(int x, int y);
	void generate(int startX, int endX);

	public:

		Scene_Game();
		void loop() override;
		void event(SDL_Event &Event) override;
		~Scene_Game();
};