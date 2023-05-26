#include "AST.h"
#include <thread>
#include <iostream>
#include <SDL_image.h>

namespace AST {
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Point Mouse;
	int grid = 1;
	bool loop;
	bool isFullscreen = false;
	std::unordered_map<std::string, bool> keys;
	std::string instruction;

	void Init(std::string title, int width, int height) {

		SDL_Init(SDL_INIT_EVERYTHING);
		win = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
		ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);
	}

	void HandleEv(SDL_Event & event) {
			bool down;

			SDL_GetMouseState(&AST::Mouse.x, &AST::Mouse.y);

			AST::Mouse.x /= AST::grid;
			AST::Mouse.y /= AST::grid;
			AST::Mouse.x *= AST::grid;
			AST::Mouse.y *= AST::grid;

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				std::string to;
				switch (event.button.button) {
				case SDL_BUTTON_LEFT:
					to = "L";
					break;
				case SDL_BUTTON_MIDDLE:
					to = "M";
					break;
				case SDL_BUTTON_RIGHT:
					to = "R";
					break;
				}	
				keys[to + "MB"] = true;
			}

			//Key Handling


			if (event.type == SDL_QUIT) {
				keys["SDL_QUIT"] = true;
				return;
			}

			// https://wiki.libsdl.org/SDL2/SDL_Keycode
			if (event.type == SDL_KEYDOWN) keys[SDL_GetKeyName(event.key.keysym.sym)] = true;
	}

	Scene::Scene() {
		AST::loop = true;
	}

	void Scene::loop(SDL_Point& mouse) {
		
	}

	void Scene::event(SDL_Event& event, SDL_Point& mouse) {
		AST::HandleEv(event);
		if (AST::keys["SDL_QUIT"]) AST::loop = false;
	}

	Scene::~Scene() {
		AST::loop = false;
		SpriteManager::free();
	}

	void Render(Scene& scene) {
		while (AST::loop) {
			//Event Handling
			SDL_Event event;
			keys.clear();
			while (SDL_PollEvent(&event)) scene.event(event, Mouse);
			//Rendering
			SDL_RenderClear(ren);
			scene.loop(Mouse);
			SDL_RenderPresent(ren);
		}
	}

	void Quit() {
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

	Rect::Rect(SDL_Rect rect, std::string texture) {
		init(rect);
		this->texture = texture;
		this->type = "TRect";
	}

	Rect::Rect(SDL_Rect rect, SDL_Color color) {
		init(rect);
		start = color;
		this->type = "CRect";
	}

	Rect::Rect(SDL_Rect rect, SDL_Color start, SDL_Color end) {
		init(rect);
		this->start = start;
		this->end = end;
		this->type = "GRect";
	}

	void Rect::init(SDL_Rect rect) {
		x = rect.x;
		y = rect.y;
		w = rect.w == -1 ? 32 * 42 : rect.w;
		h = rect.h == -1 ? 32 * 24 : rect.h;
		angle = 0.0;
	}

	bool inRange(int num, int min, int max) {
		return (num >= min && num <= max);
	}

	bool hovering(SDL_Rect rect) {
		return inRange(Mouse.x, rect.x, rect.x + rect.w) && inRange(Mouse.y, rect.y, rect.y + rect.h);
	}

	void fullscreen(bool yes) {
		SDL_SetWindowFullscreen(win, yes ? SDL_WINDOW_FULLSCREEN : 0);
	}

	void setTimeout(std::function<void()> function, int ms) {
		std::thread([function, ms]() {
			SDL_Delay(ms);
			function();
		}).detach();
	}
} // namespace AST

namespace SpriteManager {
	std::vector<std::pair<SDL_Texture*, std::string>> sprites;
	bool debug;

	bool load(std::string keyword, std::string file) {
		for (auto sprite : sprites) if (sprite.second == keyword && sprite.first != nullptr) return true;

		SDL_Surface * temp = IMG_Load(("res/" + file).c_str());

		if (temp != nullptr) {
			sprites.push_back({
				SDL_CreateTextureFromSurface(AST::ren, temp),
				keyword
			});
			SDL_FreeSurface(temp);
			return true;
		}

		return false;
	}

	bool load(std::string keyword, std::string sheetFile, SDL_Rect spriteRect) {
	    

	    for (auto sprite : sprites) if (sprite.second == keyword && sprite.first != nullptr) return true;

	    SDL_Surface * sheetSurface = IMG_Load(("res/" + sheetFile).c_str());
	    if (sheetSurface == nullptr) return false;

	    SDL_Texture * sheetTexture = SDL_CreateTextureFromSurface(AST::ren, sheetSurface);
	    SDL_FreeSurface(sheetSurface);

	    if (sheetTexture == nullptr) return false;

	    SDL_Texture* spriteTexture = SDL_CreateTexture(AST::ren, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, spriteRect.w, spriteRect.h);
	    if (spriteTexture == nullptr) {
	        SDL_DestroyTexture(sheetTexture);
	        return false;
	    }

	    SDL_SetRenderTarget(AST::ren, spriteTexture);
	    SDL_RenderCopy(AST::ren, sheetTexture, &spriteRect, nullptr);
	    SDL_SetRenderTarget(AST::ren, nullptr);
	    sprites.push_back({ spriteTexture, keyword });

	    return true;
	}


	bool drawTRect(AST::Rect rect) {
		bool found = false;

		for (auto sprite : sprites) {
			if (sprite.second == rect.texture) {
				SDL_RenderCopy(AST::ren, sprite.first, nullptr, &rect);
				// SDL_RenderCopyEx(AST::ren, sprite.first, nullptr, &rect, rect.angle, nullptr, SDL_FLIP_NONE);
				found = true;
				break;
			}
		}

		if (!found && debug) {
			std::cout << "Texture " << rect.texture << " was not found!" << std::endl;
		}

		return found;
	}

	void drawCRect(AST::Rect rect) {
		SDL_SetRenderDrawColor(AST::ren, rect.start.r, rect.start.g, rect.start.b, rect.start.a);
		SDL_RenderFillRect(AST::ren, &rect);
		SDL_SetRenderDrawColor(AST::ren, 0, 0, 0, 255);
	}

	void drawGRect(AST::Rect rect) {
		// Calculate the color step for each pixel
		float rStep = (float)(rect.end.r - rect.start.r) / rect.h;
		float gStep = (float)(rect.end.g - rect.start.g) / rect.h;
		float bStep = (float)(rect.end.b - rect.start.b) / rect.h;

		// Loop through each row of the rectangle
		for (int y = rect.y; y < rect.y + rect.h; ++y) {
			// Calculate the color for the current row
			Uint8 r = rect.start.r + (rStep * (y - rect.y));
			Uint8 g = rect.start.g + (gStep * (y - rect.y));
			Uint8 b = rect.start.b + (bStep * (y - rect.y));

			// Set the color for the current row
			SDL_SetRenderDrawColor(AST::ren, r, g, b, 255);

			// Draw a horizontal line for the current row
			SDL_RenderDrawLine(AST::ren, rect.x, y, rect.x + rect.w, y);
		}
		SDL_SetRenderDrawColor(AST::ren, 0, 0, 0, 255);
	}

	bool draw(AST::Rect rect) {
		if (rect.type == "TRect") {
			return drawTRect(rect);
		}
		else if (rect.type == "CRect") {
			drawCRect(rect);
			return true;
		}
		else if (rect.type == "GRect") {
			drawGRect(rect);
			return true;
		}
		else {
			return false;
		}
	}

	void free() {
		for (auto sprite : sprites) {
			SDL_DestroyTexture(sprite.first);
		}
		sprites.clear();
	}
} // namespace SpriteManager
