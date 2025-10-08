#include "GameEngine.h"
#include <iostream>

// function which effectively does the same game loop as setupGame() and
// runGame()
void testGameStates() {
	std::cout << "Testing Game State Functionality..." << std::endl;
	std::string stateBefore;
	try {
		// Create GameEngine instance
		GameEngine* game = new GameEngine();
		std::cout << "Created Game Engine Instance" << std::endl;

		while (true){
			std::cout << "\n* Please enter a command: ";
			std::string command;
			std::getline(std::cin, command);

			// Skip empty lines
			if(command.find_first_not_of(" \t\n\r") == std::string::npos) {
				continue;
			}

			game->command(command);
		}
		delete game;

		std::cout << "\nTest completed successfully!" << std::endl;
	} catch (const std::exception &e) {
		std::cout << "Error during Game Engine testing: " << e.what()
				  << std::endl;
	}
}
