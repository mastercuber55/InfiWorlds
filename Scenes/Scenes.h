#pragma once

#include "../AST.h"
#include "../PerlinNoise.h"
#include <string>

class Scene_Title : public AST::Scene {
	AST::Rect Background;
	public:
		Scene_Title();
		void loop(SDL_Point &Mouse) override;
		~Scene_Title();
};


class Scene_Game : public AST::Scene {

	PerlinNoise pn;
	AST::Rect Background;
	std::vector<AST::Rect> tiles;

	public:

		Scene_Game();
		void loop(SDL_Point &Mouse) override;
		// void event(SDL_Event &Event, SDL_Point &Mouse) override;
		~Scene_Game();
};