#include "CommandProcessor/CommandProcessorDriver.h"
#include "CommandProcessor/CommandProcessor.h"
#include "GameEngine/GameEngine.h"
#include <fstream>
#include <iostream>

void testCommandProcessor() {
    std::cout << "\nTesting Command Processor" << std::endl;
    std::cout << "=======================" << std::endl;

    // Create a test command file
    std::string filename = "commands.txt";
    std::ofstream testFile(filename);
    testFile << "loadmap testmap.map\n";
    testFile << "validatemap\n";
    testFile << "addplayer Player 1\n";
    testFile << "addplayer\n";
    testFile << "invalid_syntax\n";
    testFile << "gamestart\n";
    testFile.close();

    // Test Scenario 1: Console Command Processing
    std::cout << "\n1. Testing Console Command Processing:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    {
        CommandProcessor consoleProcessor;
        std::cout << "Please enter a command (e.g., 'loadmap "
                     "testmap.map'): "
                  << std::endl;
        Command* cmd1 = consoleProcessor.getCommand();
        if (cmd1) {
            std::cout << "Command entered: " << cmd1->getCommandText()
                      << std::endl;
            std::cout << "Effect: " << cmd1->getEffectText() << std::endl;
        }
        cmd1 = nullptr;
    }

    // Test Scenario 2: File Command Processing
    std::cout << "\n2. Testing File Command Processing:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    {
        try {
            // Use the same filename as created above
            FileCommandProcessorAdapter fileProcessor(filename);
            std::cout << "Reading commands from file '"
                      << fileProcessor.getFileName() << "':" << std::endl;

            while (Command* cmd = fileProcessor.getCommand()) {
                std::cout << "\nCommand read from file: '"
                          << cmd->getCommandText() << "'" << std::endl;

                fileProcessor.validate(cmd, StateType::start);
                std::cout << "----------------" << std::endl;
            }
        } catch (const std::exception &e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    // Test Scenario 3: Command Validation and State Changes
    std::cout << "\n3. Testing Command Validation and State Changes:"
              << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    {
        // Create a processor to test validation
        CommandProcessor consoleProcessor;

        // Test Case 1: Correct syntax, correct args, correct state
        StateType currentState = StateType::start;
        Command* cmd1 = new Command("loadmap test.map");
        consoleProcessor.validate(cmd1, currentState);
        std::cout << "----------------" << std::endl;

        // Test Case 2: Correct syntax, correct args, incorrect state
        Command* cmd2 = new Command("addplayer Player1");
        consoleProcessor.validate(cmd2, currentState);
        std::cout << "----------------" << std::endl;

        // Test Case 3: Correct syntax, incorrect args, correct state
        currentState = StateType::mapvalidated;
        Command* cmd3 = new Command("addplayer");
        consoleProcessor.validate(cmd3, currentState);

        cmd1 = nullptr;
        cmd2 = nullptr;
        cmd3 = nullptr;
    }

    // Cleanup
    if (std::remove("commands.txt") != 0) {
        std::cout << "Note: Could not remove test file. It may have "
                     "already been deleted."
                  << std::endl;
    } else {
        std::cout << "Test file cleanup successful." << std::endl;
    }
}