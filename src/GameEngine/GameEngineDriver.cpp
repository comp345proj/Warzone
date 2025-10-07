#include "GameEngine.h"
#include <iostream>

// function which effectively does the same game loop as setupGame() and
// runGame()
void testGameStates() {
	std::cout << "Testing Game State Functionality..." << std::endl;
	try {
		bool testingStates = true;
		while(testingStates){
		// Create a GameEngine instance
		GameEngine* game = new GameEngine();
		std::cout << "Created Game Engine " << std::endl;
		std::cout << "Current State: " << game->getState()
				  << std::endl;
		std::cout << "Type MANUAL for list of commands." << std::endl;
		std::string command;
		std::getline(std::cin, command);
		std::vector<std::string> splitCommand = splitString(command, ' ');
		if (splitCommand[0] == "QUIT") {
			testingStates = false;
			std::cout << "Exiting Game State testing." << std::endl;
			delete game;
			break;
		} else {
			game->command(command);
		}
		// Start
		//game->runGame(); // runs actual game
		delete game;
		}
	} catch (const std::exception &e) {
		std::cout << "Error during Game Engine testing: " << e.what()
				  << std::endl;
	}
}
