#include "CommandProcessing/CommandProcessingDriver.h"
#include "CommandProcessing/CommandProcessing.h"
#include "GameEngine/GameEngine.h"
#include <iostream>

void testCommandProcessor(int argc, char* argv[]) {
	std::cout << "\n=== Command Processor Test Driver ===" << std::endl;

	if (argc <= 1) {
		std::cout << "No arguments provided. Usage: ./program [-console] OR "
					 "[-file filename]"
				  << std::endl;
		return;
	}

	std::string commandOption = argv[1];
	std::string filename;

	if (commandOption == "-file") {
		if (argc != 3) {
			std::cout << "Error: -file option requires a filename" << std::endl;
			return;
		}
		filename = argv[2];

		// Create a test file with commands
		std::ofstream testFile(filename);
		testFile << "loadmap europe.map\n";
		testFile << "validatemap\n";
		testFile << "addplayer Alice\n";
		testFile << "addplayer Bob\n";
		testFile << "gamestart\n";
		testFile << "quit\n";
		testFile.close();

		std::cout << "\nTesting File Command Processor Adapter..." << std::endl;
		std::cout << "Reading commands from file: " << filename << std::endl;

		try {
			FileCommandProcessorAdapter fcpa(filename);
			StateType currentState = StateType::START;

			// Process all commands in the file
			while (true) {
				try {
					fcpa.readCommand();
					Command &cmd = fcpa.getCommand();

					std::cout << "\nRead command: " << cmd.getCommand()
							  << std::endl;
					bool isValid = fcpa.validate(cmd, currentState);

					std::cout << "Valid in current state: "
							  << (isValid ? "yes" : "no") << std::endl;
					std::cout << "Effect: " << cmd.getEffect() << std::endl;

					// Update state based on command
					if (isValid) {
						if (cmd.getCommand().find("loadmap") == 0)
							currentState = StateType::MAP_LOADED;
						else if (cmd.getCommand() == "validatemap")
							currentState = StateType::MAP_VALIDATED;
						else if (cmd.getCommand().find("addplayer") == 0 &&
								 currentState == StateType::MAP_VALIDATED)
							currentState = StateType::PLAYERS_ADDED;
						else if (cmd.getCommand() == "gamestart")
							currentState = StateType::ASSIGN_REINFORCEMENT;
						else if (cmd.getCommand() == "quit")
							break;
					}

				} catch (const std::runtime_error &e) {
					std::cout << "Reached end of file" << std::endl;
					break;
				}
			}

		} catch (const std::runtime_error &e) {
			std::cout << "Error: " << e.what() << std::endl;
		}

		// Clean up test file
		remove(filename.c_str());

	} else if (commandOption == "-console") {
		std::cout << "\nTesting Console Command Processor..." << std::endl;
		CommandProcessor cp;
		StateType currentState = StateType::START;

		// Simulate console input
		std::vector<std::string> testCommands = {
			"loadmap europe.map", "invalidcommand", "validatemap",
			"addplayer Alice", "quit"};

		for (const auto &cmd : testCommands) {
			std::cout << "\nEntering command: " << cmd << std::endl;
			cp.saveCommand(cmd);
			Command &command = cp.getCommand();

			bool isValid = cp.validate(command, currentState);
			std::cout << "Valid in current state: " << (isValid ? "yes" : "no")
					  << std::endl;
			std::cout << "Effect: " << command.getEffect() << std::endl;

			// Update state based on command
			if (isValid) {
				if (cmd.find("loadmap") == 0)
					currentState = StateType::MAP_LOADED;
				else if (cmd == "validatemap")
					currentState = StateType::MAP_VALIDATED;
				else if (cmd.find("addplayer") == 0)
					currentState = StateType::PLAYERS_ADDED;
				else if (cmd == "quit")
					break;
			}
		}

	} else {
		std::cout << "Invalid option. Use -console or -file <filename>"
				  << std::endl;
		return;
	}

	std::cout << "\n=== Command Processor Test Complete ===" << std::endl;
	// while (!game->isGameOver()) {
	// 	Command& command = game->getCommandProcessor().getCommand();

	// 	if (command.getCommand() == "EOD")
	// 	{
	// 		std::cout << "End of commands reached." << std::endl;
	// 		break;
	// 	}

	// 	if (game->getCommandProcessor().validate(command, game->getState()))
	// 	{

	// 	}

	// }
}