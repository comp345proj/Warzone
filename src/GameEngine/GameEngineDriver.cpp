#include "GameEngine.h"
#include <iostream>

void testGameStates() {
	std::cout << "Testing Game State Functionality..." << std::endl;
	try {
		// Create a GameEngine instance
		GameEngine* game = new GameEngine();
		std::cout << "Created Game Engine " << std::endl;

		bool isGameRunning = true;
		while (!game->isGameOver()) {
			std::cout << "Game is running..." << std::endl;
			std::cout << "\nCurrent State: " << game->getCurrentStateName()
					  << std::endl;
			std::string command;
			std::cout << "Enter command or type 'MANUAL' to view possible commands: ";

			if (!std::getline(std::cin, command)) {
				std::cout << "Error reading command. Exiting." << std::endl;
				break;
			}

			game->command(command);
		}
		std::cout << "Game Over!" << std::endl;
		delete game;
	} catch (const std::exception &e) {
		std::cout << "Error during Game Engine testing: " << e.what()
				  << std::endl;
	}
}
