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

			game->command(command);
		}

		/*
		// Test opening state
		std::cout << "\nTesting Opening State:" << std::endl;
		game->command("STATE");

		// Test manual command
		std::cout << "\nTesting Manual Command:" << std::endl;
		game->command("MANUAL");

		// Test Load Map command and 'MapLoaded' state transition
		std::cout
			<< "\nTesting Load Map Command and 'MapLoaded' state transition:"
			<< std::endl;
		stateBefore = game->getState();
		game->command("LOADMAP DEFAULT");
		std::cout << "State Before: " << stateBefore << "\nState After: " << game->getState() << std::endl;

		// Test Validate Map command and 'MapValidated' state transition
		std::cout << "\nTesting Validate Map Command and 'MapValidated' state "
					 "transition:"
				  << std::endl;
		stateBefore = game->getState();
		game->command("VALIDATEMAP");
		std::cout << "State Before: " << stateBefore << "\nState After: " << game->getState() << std::endl;

		// Test Add Player command and 'PlayersAdded' state transition
		std::cout << "\nTesting Add Player Command and 'PlayersAdded' state "
					 "transition:"
				  << std::endl;
		stateBefore = game->getState();
		game->command("ADDPLAYER Josuke");
		game->command("ADDPLAYER Giorno");
		game->command("ADDPLAYER Jolyne");
		std::cout << "State Before: " << stateBefore << "\nState After: " << game->getState() << std::endl;

		// Test Start command and 'AssignReinforcement' state transition
		std::cout << "\nTesting Game Start Command and 'AssignReinforcement' "
					 "state transition:"
				  << std::endl;
		stateBefore = game->getState();
		game->command("GAMESTART");
		std::cout << "State Before: " << stateBefore << "\nState After: " << game->getState() << std::endl;

		// Set Game Win to true to simulate game replay to 'Start' state
		// transition
		std::cout << "\nTesting Replay Command and 'Start' state transition:"
				  << std::endl;
		game->setState("Win");
		stateBefore = game->getState();
		game->command("REPLAY");
		std::cout << "State Before: " << stateBefore << "\nState After: " << game->getState() << std::endl;

		// Test Quit command and 'Exit' state transition
		std::cout << "\nTesting Quit Command and 'Exit' state transition:"
				  << std::endl;
		game->setState("Win");
		stateBefore = game->getState();
		game->command("QUIT");
		std::cout << "Is Game Over Function check: " << game->isGameOver() << std::endl;
		std::cout << "State Before: " << stateBefore << "\nState After: " << game->getState() << std::endl;
		*/
		// Clean up
		delete game;

		std::cout << "\nTest completed successfully!" << std::endl;
	} catch (const std::exception &e) {
		std::cout << "Error during Game Engine testing: " << e.what()
				  << std::endl;
	}
}
