#include "CommandProcessor/CommandProcessor.h"
#include "GameEngine.h"
#include "LoggingObserver/LoggingObserver.h"
#include <iostream>

void testTournament() {
    std::cout << "\n=== Testing Tournament Mode ===\n" << std::endl;

    // Create command processor
    CommandProcessor* cmdProcessor = new CommandProcessor();

    // Create game engine
    GameEngine* gameEngine = new GameEngine(cmdProcessor);

    // Create a log observer
    LogObserver* logObserver = new LogObserver();
    gameEngine->Attach(logObserver);

    std::cout << "\nTesting tournament command processing..." << std::endl;

    std::string tournamentCmd = "tournament -M Moon.map,Venus.map -P "
                                "Aggressive,Benevolent,Neutral -G 5 -D 50";

    Command cmd(tournamentCmd);
    std::cout << "Tournament command: '" << cmd.getCommandText() << "'"
              << std::endl;

    if (cmdProcessor->validate(&cmd, StateType::start)) {
        // Execute tournament
        try {
            Tournament tournament =
                cmdProcessor->prepareTournament(cmd.getCommandText());
            gameEngine->runTournament(tournament);
            std::cout << "Tournament executed successfully." << std::endl;
        } catch (const std::exception &e) {
            std::cout << "Error executing tournament: " << e.what()
                      << std::endl;
        }
    }

    // Cleanup
    delete cmdProcessor;
    delete gameEngine;
    delete logObserver;
}
