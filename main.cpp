#include "AST.h"
#include "Scenes/Scenes.h"
#include <ctime>

int main(int argc, char *argv[]) {
	AST::Init("InfiWorlds2");
	srand(time(nullptr));
	// {
	// 	Scene_Title TitleScene;
	// 	AST::Render(TitleScene);
	// }
	{
		Scene_Game GameScene;
		AST::Render(GameScene);
	}

	AST::Quit();
	return false;
}
