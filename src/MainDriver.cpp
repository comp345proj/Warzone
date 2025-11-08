#include "Cards/Cards.h"
#include "Cards/CardsDriver.h"
#include "CommandProcessor/CommandProcessorDriver.h"
#include "GameEngine/GameEngine.h"
#include "GameEngine/GameEngineDriver.h"
#include "Map/Map.h"
#include "Map/MapDriver.h"
#include "Observer/LoggingObserverDriver.h"
#include "Orders/Orders.h"
#include "Orders/OrdersDriver.h"
#include "Player/Player.h"
#include "Player/PlayerDriver.h"
#include "Utils/Utils.h"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		std::cout << "No arguments provided. Usage: ./WarzoneTestDriver "
					 "[-console] OR [-file filename]"
				  << std::endl;
		return 1;
	}
	if (argc >= 4) {
		std::cout << "Too many arguments provided. Usage: ./WarzoneTestDriver "
					 "[-console] OR [-file filename]"
				  << std::endl;
		return 1;
	}

	// std::string commandOption;
	// std::string filename;

	// for (int i = 0; i < argc; ++i)
	// {
	//     std::cout << "Argument " << i << ": " << argv[i] << std::endl;
	//     if (i == 1)
	//     {
	//         commandOption = argv[i];
	//     }
	//     else if (i == 2)
	//     {
	//         filename = argv[i];
	//     }
	// }

	// if (commandOption == "-file")
	// {
	//     // Load file as input
	// }

	std::cout << "Warzone Game Test Driver\n"
			  << std::endl
			  << SEPARATOR_LINE << std::endl
			  << "Map Tests\n"
			  << std::endl;

	testLoadMaps();

	std::cout << SEPARATOR_LINE << std::endl << "Cards Tests\n" << std::endl;
	testCards();

	std::cout << SEPARATOR_LINE << std::endl << "Orders Tests\n" << std::endl;
	testOrdersList();

	std::cout << SEPARATOR_LINE << std::endl << "Player Tests\n" << std::endl;
	testPlayers();

	std::cout << SEPARATOR_LINE << std::endl;
	std::cout << "ASSIGNMENT 2" << std::endl;
	std::cout << SEPARATOR_LINE << std::endl;
	
	std::cout << "Command Processing Tests\n" << std::endl;
	testCommandProcessor(argc, argv);
	
	// std::cout << SEPARATOR_LINE << std::endl;
	// std::cout << "Logging Observer Tests\n" << std::endl;
	// testLoggingObserver();

	std::cout << SEPARATOR_LINE << std::endl
			  << "Game Engine Tests\n"
			  << std::endl;
	testGameStates();

	return 0;
}