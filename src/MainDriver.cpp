#include "Cards/Cards.h"
#include "Cards/CardsDriver.h"
#include "CommandProcessor/CommandProcessorDriver.h"
#include "GameEngine/GameEngine.h"
#include "GameEngine/GameEngineDriver.h"
#include "LoggingObserver/LoggingObserverDriver.h"
#include "Map/Map.h"
#include "Map/MapDriver.h"
#include "Orders/Orders.h"
#include "Orders/OrdersDriver.h"
#include "Player/Player.h"
#include "Player/PlayerDriver.h"
#include "PlayerStrategies/PlayerStrategiesDriver.h"
#include "Utils/Utils.h"
#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {

    // std::cout << "\n" << SEPARATOR_LINE << std::endl;
    // std::cout << "ASSIGNMENT 1" << std::endl;
    // std::cout << SEPARATOR_LINE << "\n" << std::endl;

    // testLoadMaps();
    // std::cout << SEPARATOR_LINE << std::endl;

    // testCards();
    // std::cout << SEPARATOR_LINE << std::endl;
    
	// testOrdersList();
    // std::cout << SEPARATOR_LINE << std::endl;

    // testPlayers();
    // std::cout << SEPARATOR_LINE << std::endl;

	// -----------------------------------------------

    // std::cout << "\n" << SEPARATOR_LINE << std::endl;
    // std::cout << "ASSIGNMENT 2" << std::endl;
    // std::cout << SEPARATOR_LINE << "\n" << std::endl;

    // testCommandProcessor();
    // std::cout << SEPARATOR_LINE << std::endl;
    
	// testStartupPhase();
    // std::cout << SEPARATOR_LINE << std::endl;

    // testMainGameLoop();
    // std::cout << SEPARATOR_LINE << std::endl;
	
    // testOrderExecution();
    // std::cout << SEPARATOR_LINE << std::endl;
    
	// testLoggingObserver();
    // std::cout << SEPARATOR_LINE << std::endl;

    // -----------------------------------------------

    std::cout << "\n" << SEPARATOR_LINE << std::endl;
    std::cout << "ASSIGNMENT 3" << std::endl;
    std::cout << SEPARATOR_LINE << "\n" << std::endl;

    testPlayerStrategies();
    std::cout << SEPARATOR_LINE << std::endl;

	testTournament();
	std::cout << SEPARATOR_LINE << std::endl;

    // -----------------------------------------------

    // Run game without tests:

    // std::cout << SEPARATOR_LINE << std::endl
    //           << "\nGame without tests\n"
    //           << std::endl;
    // testGame(argc, argv);

    return 0;
}