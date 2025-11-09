#include "GameEngine.h"
#include <iostream>

// Driver function to test the main game loop
void testGameStates(int argc, char* argv[]) {
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
