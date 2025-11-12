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

int main(int argc, char* argv[]) {

    // std::cout << SEPARATOR_LINE << std::endl;
    // std::cout << "ASSIGNMENT 1" << std::endl;
    // std::cout << SEPARATOR_LINE << std::endl;

    // std::cout << "Map Tests\n" << std::endl;
    // testLoadMaps();

    // std::cout << SEPARATOR_LINE << std::endl << "Cards Tests\n" << std::endl;
    // testCards();

    // std::cout << SEPARATOR_LINE << std::endl << "Orders Tests\n" <<
    // std::endl; testOrdersList();

    // std::cout << SEPARATOR_LINE << std::endl << "Player Tests\n" <<
    // std::endl; testPlayers();

    // std::cout << SEPARATOR_LINE << std::endl;
    // std::cout << "ASSIGNMENT 2" << std::endl;
    // std::cout << SEPARATOR_LINE << std::endl;

    // std::cout << SEPARATOR_LINE << std::endl;
    // std::cout << "Command Processing Tests\n" << std::endl;
    // testCommandProcessor();

    // std::cout << SEPARATOR_LINE << std::endl;
    // std::cout << "Startup Phase Tests\n" << std::endl;
    // testStartupPhase();

    // std::cout << SEPARATOR_LINE << std::endl;
    // std::cout << "Main Game Loop Tests\n" << std::endl;
    // testMainGameLoop();

    // std::cout << SEPARATOR_LINE << std::endl;
    // std::cout << "Order Execution Tests\n" << std::endl;
    // testOrderExecution();

    // std::cout << SEPARATOR_LINE << std::endl;
    // std::cout << "Logging Observer Tests\n" << std::endl;
    // testLoggingObserver();

    std::cout << SEPARATOR_LINE << std::endl;
    std::cout << "ASSIGNMENT 3" << std::endl;
    std::cout << SEPARATOR_LINE << std::endl;

    std::cout << SEPARATOR_LINE << std::endl;
    std::cout << "Player Strategies Tests\n" << std::endl;
    testPlayerStrategies();

    // To run an actual game without tests, uncomment below
    //
    // std::cout << SEPARATOR_LINE << std::endl
    //           << "Actual Game Test\n"
    //           << std::endl;
    // testGame(argc, argv);

    return 0;
}