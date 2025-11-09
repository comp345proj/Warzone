#include "GameEngine.h"
#include <iostream>

// Driver function to test the startup phase
// Demonstrates: (1) map loading and validation, (2) player creation,
// (3) territory distribution, (4) initial army allocation,
// (5) initial card draw, (6) turn order determination
void testStartupPhase() {
    std::cout << "\n=== Testing Startup Phase ===" << std::endl;
    
    // Create command processor with file input for automated testing
    CommandProcessor* cmdProcessor = new FileCommandProcessorAdapter("commands.txt");
    
    GameEngine* gameEngine = new GameEngine(cmdProcessor);
    
    std::cout << "\nStarting startup phase..." << std::endl;
    std::cout << "This will demonstrate:" << std::endl;
    std::cout << "1. Loading and validating a map" << std::endl;
    std::cout << "2. Creating players" << std::endl;
    std::cout << "3. Distributing territories fairly" << std::endl;
    std::cout << "4. Allocating initial armies (50 per player)" << std::endl;
    std::cout << "5. Drawing initial cards (2 per player)" << std::endl;
    std::cout << "6. Randomizing turn order" << std::endl;
    
    // Execute startup phase
    gameEngine->startupPhase(false);
    
    std::cout << "\n=== Startup Phase Complete ===" << std::endl;
    
    delete gameEngine;
    delete cmdProcessor;
}

// Driver function to test the main game loop
// Demonstrates: (1) correct reinforcement calculation, (2) deploy orders mandatory,
// (3) advance orders from toAttack()/toDefend(), (4) playing cards,
// (5) player elimination, (6) win condition
void testMainGameLoop() {
    std::cout << "\n=== Testing Main Game Loop ===" << std::endl;
    
    // Create command processor with file input for automated testing
    CommandProcessor* cmdProcessor = new FileCommandProcessorAdapter("commands.txt");
    
    GameEngine* gameEngine = new GameEngine(cmdProcessor);
    
    std::cout << "\nThis test demonstrates:" << std::endl;
    std::cout << "1. Reinforcement calculation based on territories and continents" << std::endl;
    std::cout << "2. Players MUST issue deploy orders before other orders" << std::endl;
    std::cout << "3. Players can issue advance orders to attack/defend" << std::endl;
    std::cout << "4. Players can play cards to issue special orders" << std::endl;
    std::cout << "5. Players without territories are eliminated" << std::endl;
    std::cout << "6. Game ends when one player controls all territories" << std::endl;
    
    // Execute startup phase first
    gameEngine->loadMap("World.map");
	gameEngine->validateMap();
	gameEngine->addPlayer("Alice");
	gameEngine->addPlayer("Bob");
	gameEngine->gameStart(false);

	std::cout << "\nStarting main game loop..." << std::endl;
    gameEngine->mainGameLoop(false);
    
    std::cout << "\n=== Main Game Loop Complete ===" << std::endl;
    
    delete gameEngine;
    delete cmdProcessor;
}

// Driver function to test the main game loop
void testGame(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "No arguments provided. Usage: ./WarzoneTestDriver "
                     "[-console] OR [-file filename]"
                  << std::endl;
        return;
    }
    if (argc >= 4) {
        std::cout << "Too many arguments provided. Usage: "
                     "./WarzoneTestDriver "
                     "[-console] OR [-file filename]"
                  << std::endl;
        return;
    }

    std::string commandOption = argv[1];
    CommandProcessor* cmdProcessor = nullptr;

    if (commandOption == "-console") {
        cmdProcessor = new CommandProcessor();
    } else if (commandOption == "-file") {
        if (argc != 3) {
            std::cout << "Error: -file option requires a filename" << std::endl;
            return;
        }
        std::string filename = argv[2];
        cmdProcessor = new FileCommandProcessorAdapter(filename);
    } else {
        std::cout << "Invalid option. Use -console or -file filename"
                  << std::endl;
        return;
    }

    GameEngine* gameEngine = new GameEngine(cmdProcessor);
    gameEngine->startupPhase();
    delete gameEngine;
    delete cmdProcessor;
}
