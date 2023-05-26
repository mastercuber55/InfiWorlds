#include "Scenes.h"

Scene_Game::Scene_Game() : Background({0, 0, -1, -1}, "Background"), pn(rand() * rand()) {

	SpriteManager::load("Background", SCENE("Background/Background.png"));
	SpriteManager::load("GrassBegin", SCENE("Assets/Tiles.png"), {0, 0, 32, 32});
	SpriteManager::load("Grass", SCENE("Assets/Tiles.png"), {16, 0, 32, 32});
	SpriteManager::load("GrassEnd", SCENE("Assets/Tiles.png"), {48, 0, 32, 32});
	SpriteManager::load("Dirt", SCENE("Assets/Tiles.png"), {48, 32, 16, 16});

	for (int x = 0; x < 42; ++x) {
	    double noise = pn.noise(x / 24.0, 0.0);  // Generate noise for the x-coordinate only

	    // Adjust the y-coordinate based on the noise value to create a platform
	    int platformY = static_cast<int>(noise * 24);

	    for (int y = platformY; y < 24; ++y) {
	        tiles.push_back(AST::Rect({ x * 32, y * 32, 32, 32 }, (y == platformY) ? "Grass" : "Dirt"));
	    }
	}
}

void Scene_Game::loop(SDL_Point &Mouse) {
	SpriteManager::draw(Background);
	for(auto& tile : tiles) SpriteManager::draw(tile);
}

Scene_Game::~Scene_Game() {
	SpriteManager::free();
}