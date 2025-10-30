#include "Cards/Cards.h"
#include "Cards/CardsDriver.h"
#include "Map/Map.h"
#include "Map/MapDriver.h"
#include "Orders/Orders.h"
#include "Orders/OrdersDriver.h"
#include "Player/Player.h"
#include "Player/PlayerDriver.h"
#include "GameEngine/GameEngine.h"
#include "GameEngine/GameEngineDriver.h"
#include "Utils/Utils.h"
#include <iostream>

int main(int argc, char* argv[]) {
	std::string commandOption;
    std::string filename;

    for (int i = 0; i < argc; ++i)
    {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
        if (i == 1)
        {
            commandOption = argv[i];
        }
        else if (i == 2)
        {
            filename = argv[i];
        }
    }

    if (commandOption == "-file")
    {
        // Load file as input
    }

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

	std::cout << SEPARATOR_LINE << std::endl << "Game Engine Tests\n" << std::endl;
	testGameStates();

	std::cout << SEPARATOR_LINE << std::endl;

	return 0;
}