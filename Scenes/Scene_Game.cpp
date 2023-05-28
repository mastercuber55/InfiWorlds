#include "Scenes.h"
#include <iostream>

Scene_Game::Scene_Game() : Background({0, 0, -1, -1}, "Background"), pn(rand() * rand()), cords{0, 0} {

	SpriteManager::load("Background", SCENE("Background/Background.png"));
	SpriteManager::load("GrassBegin", SCENE("Assets/Tiles.png"), {0, 0, 32, 32});
	SpriteManager::load("Grass", SCENE("Assets/Tiles.png"), {16, 0, 32, 32});
	SpriteManager::load("GrassEnd", SCENE("Assets/Tiles.png"), {48, 0, 32, 32});
	SpriteManager::load("Dirt", SCENE("Assets/Tiles.png"), {48, 32, 16, 16});

	for (int x = cords.x; x < cords.x + 42; ++x) {
	    double noise = pn.noise(x / 24.0, 0.0);  // Generate noise for the x-coordinate only

	    // Adjust the y-coordinate based on the noise value to create a platform
	    int platformY = static_cast<int>(noise * 24);

	    for (int y = cords.y; y < cords.y + 24; ++y) {
	        std::string texture;
	        if (y == platformY) {
	            double prevNoise = pn.noise((x - 1) / 24.0, 0.0);
	            int prevplatformY = static_cast<int>(prevNoise * 24);
	            double nextNoise = pn.noise((x + 1) / 24.0, 0.0);
	            int nextplatformY = static_cast<int>(nextNoise * 24);

	            if (prevplatformY == platformY && nextplatformY == platformY)
	                texture = "Grass";
	            else if (prevplatformY == platformY && nextplatformY != platformY)
	                texture = "GrassEnd";
	            else if (prevplatformY != platformY && nextplatformY == platformY)
	                texture = "GrassBegin";
	            else
	                texture = "Grass";

	        } else if (y < platformY) {
	            texture = "Air"; // Assign "Air" texture to tiles below a threshold
	        } else {
	            texture = "Dirt"; // Assign "Dirt" texture to other tiles
	        }

	        tiles.push_back(AST::Rect({ (x - cords.x) * 32, (y - cords.y) * 32, 32, 32 }, texture));
	    }
}
}

bool outside(AST::Rect rect) {
	return (rect.x < 0 || rect.x > 32 * 42 || rect.y < 0 || rect.y > 32 * 24);
}

void Scene_Game::scroll(int x, int y) {
	cords.x += x, cords.y += y;

	x *= 32, y *= 32;

	for(auto& tile : tiles) {
		tile.x += x;
		tile.y += y;
		if(outside(tile)) {
			tile.x = tile.x < 0 ? 32 * 41 : 0;

			double noise = pn.noise((cords.x + tile.x / 32)/ 24.0, 0.0);
			int platformY = static_cast<int>(noise * 24);

			std::string& texture = tile.texture;
	        if (y == platformY) {
	            double prevNoise = pn.noise((x - 1) / 24.0, 0.0);
	            int prevplatformY = static_cast<int>(prevNoise * 24);
	            double nextNoise = pn.noise((x + 1) / 24.0, 0.0);
	            int nextplatformY = static_cast<int>(nextNoise * 24);

	            if (prevplatformY == platformY && nextplatformY == platformY)
	                texture = "Grass";
	            else if (prevplatformY == platformY && nextplatformY != platformY)
	                texture = "GrassEnd";
	            else if (prevplatformY != platformY && nextplatformY == platformY)
	                texture = "GrassBegin";
	            else
	                texture = "Grass";

	        } else if (y < platformY) {
	            texture = "Air"; // Assign "Air" texture to tiles below a threshold
	        } else {
	            texture = "Dirt"; // Assign "Dirt" texture to other tiles
	        }
		}
	}
}

void Scene_Game::loop(SDL_Point &Mouse) {
	SpriteManager::draw(Background);
	for(auto& tile : tiles) SpriteManager::draw(tile);
}

void Scene_Game::event(SDL_Event &ev, SDL_Point &Mouse) {
	AST::HandleEv(ev);
	if(AST::keys["W"]) scroll(0, 1);
	else if(AST::keys["S"]) scroll(0, -1);
	else if(AST::keys["A"]) scroll(1, 0);
	else if(AST::keys["D"]) scroll(-1, 0);
	else if(AST::keys["Escape"] || AST::keys["SDL_QUIT"]) AST::loop = false;
}

Scene_Game::~Scene_Game() {
	SpriteManager::free();
}