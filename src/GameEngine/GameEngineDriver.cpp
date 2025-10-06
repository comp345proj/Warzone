#include "GameEngine.h"
#include <iostream>

// function which effectively does the same game loop as setupGame() and
// runGame()
void testGameStates() {
	std::cout << "Testing Game State Functionality..." << std::endl;
	try {
		// Create a GameEngine instance
		GameEngine* game = new GameEngine();
		std::cout << "Created Game Engine " << std::endl;
		// Start
		game->setupGame();
		delete game;
	} catch (const std::exception &e) {
		std::cout << "Error during Game Engine testing: " << e.what()
				  << std::endl;
	}
}
