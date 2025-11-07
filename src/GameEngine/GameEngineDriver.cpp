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

		while (true) {
			std::cout << "\n* Please enter a command: ";
			std::string command;
			std::getline(std::cin, command);

			// Skip empty lines
			if (command.find_first_not_of(" \t\n\r") == std::string::npos) {
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

// Test function to demonstrate the startup phase functionality
void testStartupPhase() {
	std::cout << "\n=== Testing Startup Phase ===" << std::endl;
	try {
		// Create a new game instance
		GameEngine* game = new GameEngine();

		std::cout << "\nDemonstrating startup phase with automated commands..."
				  << std::endl;

		// 1. Test loadmap command
		std::cout << "\n1. Testing loadmap command..." << std::endl;
		game->command("loadmap World.map");

		// 2. Test validatemap command
		std::cout << "\n2. Testing validatemap command..." << std::endl;
		game->command("validatemap");

		// 3. Test addplayer command (adding 3 players)
		std::cout << "\n3. Testing addplayer command..." << std::endl;
		game->command("addplayer Player1");
		game->command("addplayer Player2");
		game->command("addplayer Player3");

		// 4. Test gamestart command
		std::cout << "\n4. Testing gamestart command..." << std::endl;
		game->command("gamestart");

		// Clean up
		delete game;
		std::cout << "\nStartup phase testing completed successfully!"
				  << std::endl;

	} catch (const std::exception &e) {
		std::cout << "Error during startup phase testing: " << e.what()
				  << std::endl;
	}
}
