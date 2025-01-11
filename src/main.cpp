#include <src/game_engine/game.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define SETTINGS_PATH "./settings.ini"


int main()
{	
	Game game;
	game.init(SETTINGS_PATH);
	game.start();
	return 0;
}