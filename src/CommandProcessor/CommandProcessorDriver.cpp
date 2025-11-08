#include "CommandProcessor/CommandProcessorDriver.h"
#include "CommandProcessor/CommandProcessor.h"
#include "GameEngine/GameEngine.h"
#include <fstream>
#include <iostream>

void testCommandProcessor(int argc, char* argv[]) {
    std::cout << "\nTesting Command Processor" << std::endl;
    std::cout << "=======================" << std::endl;

    // Create a test command file
    std::ofstream testFile("test_commands.txt");
    testFile << "loadmap testmap.map\n";
    testFile << "validatemap\n";
    testFile << "addplayer Player1\n";
    testFile << "addplayer Player2\n";
    testFile << "invalid_command\n";  // Invalid command to test rejection
    testFile << "gamestart\n";
    testFile.close();

    // Test Scenario 1: Console Command Processing
    std::cout << "\n1. Testing Console Command Processing:" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    {
        CommandProcessor consoleProcessor;
        std::cout << "Please enter a command (e.g., 'loadmap testmap.map'): " << std::endl;
        Command* cmd = consoleProcessor.getCommand();
        if (cmd) {
            std::cout << "Command entered: " << cmd->getCommandText() << std::endl;
            std::cout << "Effect: " << cmd->getEffectText() << std::endl;
        }
    }

    // Test Scenario 2: File Command Processing
    std::cout << "\n2. Testing File Command Processing:" << std::endl;
    std::cout << "---------------------------------" << std::endl;
    {
        try {
            FileCommandProcessorAdapter fileProcessor("test_commands.txt");
            std::cout << "Reading commands from file 'test_commands.txt':" << std::endl;
            
            // Process all commands in the file
            while (Command* cmd = fileProcessor.getCommand()) {
                std::cout << "\nCommand read: " << cmd->getCommandText() << std::endl;
                std::cout << "Effect: " << cmd->getEffectText() << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    // Test Scenario 3: Command Validation and State Changes
    std::cout << "\n3. Testing Command Validation and State Changes:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    {
        // Create a processor to test validation
        CommandProcessor processor;
        
        // Test Case 1: Valid command in start state
        StateType currentState = StateType::start;
        std::string command1 = "loadmap test.map";
        std::cout << "\nState: " << stateTypeToString(currentState) << std::endl;
        std::cout << "Command: " << command1 << std::endl;
        if (processor.validate(command1, currentState)) {
            std::cout << "Result: Valid command - would transition to maploaded state" << std::endl;
        } else {
            std::cout << "Result: Invalid command for current state" << std::endl;
        }

        // Test Case 2: Invalid command in start state
        std::string command2 = "addplayer Player1";
        std::cout << "\nState: " << stateTypeToString(currentState) << std::endl;
        std::cout << "Command: " << command2 << std::endl;
        if (processor.validate(command2, currentState)) {
            std::cout << "Result: Valid command" << std::endl;
        } else {
            std::cout << "Result: Invalid command - addplayer not allowed in start state" << std::endl;
        }

        // Test Case 3: Valid command in mapvalidated state
        currentState = StateType::mapvalidated;
        std::cout << "\nState: " << stateTypeToString(currentState) << std::endl;
        std::cout << "Command: " << command2 << std::endl;
        if (processor.validate(command2, currentState)) {
            std::cout << "Result: Valid command - would add player and potentially transition to playeradded state" << std::endl;
        } else {
            std::cout << "Result: Invalid command for current state" << std::endl;
        }
    }

    // Cleanup
    std::remove("test_commands.txt");
}