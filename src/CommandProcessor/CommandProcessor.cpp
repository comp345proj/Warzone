#include "CommandProcessor.h"
#include "Utils/Utils.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>

//---------------------------Command-------------------------------
Command::Command() = default;

Command::Command(const std::string &cmd) {
    std::string trimmedCmd = cmd;
    // Remove leading/trailing spaces
    trimmedCmd.erase(0, trimmedCmd.find_first_not_of(" \t"));
    trimmedCmd.erase(trimmedCmd.find_last_not_of(" \t") + 1);

    // Replace multiple spaces with single space
    size_t pos;
    while ((pos = trimmedCmd.find("  ")) != std::string::npos) {
        trimmedCmd.replace(pos, 2, " ");
    }

    // Stores full command text (including args)
    commandText = new std::string(trimmedCmd);

    // Command type is the first fragment before a space
    commandType =
        stringToCommandType(commandText->substr(0, commandText->find(' ')));

    // Assumed effect (if command is valid)
    effectText = new std::string(commandEffect[commandType]);
}

Command::Command(const Command &other)
    : commandText(new std::string(*(other.commandText))),
      effectText(new std::string(*(other.effectText))),
      commandType(other.commandType) {}

Command &Command::operator=(const Command &other) {
    if (this != &other) {
        delete commandText;
        delete effectText;
        commandText = new std::string(*(other.commandText));
        effectText = new std::string(*(other.effectText));
        commandType = other.commandType;
    }
    return *this;
}

Command::~Command() {
    delete commandText;
    delete effectText;
}

std::string &Command::getCommandText() const {
    return *commandText;
}

std::string &Command::getEffectText() const {
    return *effectText;
}

CommandType Command::getCommandType() const {
    return commandType;
}

std::string Command::stringToLog() {
    return "'" + getCommandText() + "'" + " | Effect: " + getEffectText();
}

void Command::saveEffect(const std::string &effect) {
    *effectText = effect;
    Notify(this);
}

std::ostream &operator<<(std::ostream &os, const Command &command) {
    os << "Command: " << *(command.commandText)
       << "\nEffect: " << *(command.effectText);
    return os;
}

//---------------------------CommandProcessor----------------------
CommandProcessor::CommandProcessor() {
    commandsList = std::vector<Command*>();
};

CommandProcessor::CommandProcessor(const CommandProcessor &commandProcessor) {
    commandsList = std::vector<Command*>();
    for (const auto &cmd : commandProcessor.commandsList) {
        commandsList.push_back(new Command(*cmd));
    }
}

CommandProcessor &CommandProcessor::operator=(const CommandProcessor &rhs) {
    if (this != &rhs) {
        for (auto &cmd : commandsList) {
            delete cmd;
        }
        commandsList.clear();
        for (const auto &cmd : rhs.commandsList) {
            commandsList.push_back(new Command(*cmd));
        }
    }
    return *this;
}

CommandProcessor::~CommandProcessor() {
    for (auto &cmd : commandsList) {
        delete cmd;
    }
    commandsList.clear();
}

Command* CommandProcessor::getCommand() {
    return readCommand();
}

std::vector<Command*>* CommandProcessor::getCommandsList() {
    return &commandsList;
}

std::string CommandProcessor::stringToLog() {
    return "Latest command: "
        + (commandsList.empty() ? "No commands processed yet."
                                : commandsList.back()->stringToLog());
}

// Prompts user to enter command (console mode)
Command* CommandProcessor::readCommand() {
    std::string input;
    std::cout << "\nEnter command: ";
    std::getline(std::cin, input);
    if (input.empty()) {
        std::getline(std::cin, input);
    }
    Command* cmd = new Command(input);
    saveCommand(cmd);
    return cmd;
}

bool CommandProcessor::validate(Command* command, StateType state, bool print) {

    // 1- Check if command syntax is valid (first fragment)
    bool isValidSyntax = command->getCommandType() != CommandType::invalid;

    size_t expectedArgsCount = getCommandArgsCount(command->getCommandType());
    size_t actualArgsCount = 0;

    size_t pos = command->getCommandText().find(' ');
    if (pos != std::string::npos) {
        std::string argsStr = command->getCommandText().substr(pos + 1);
        argsStr.erase(0, argsStr.find_first_not_of(" \t"));
        argsStr.erase(argsStr.find_last_not_of(" \t") + 1);

        if (!argsStr.empty()) {
            actualArgsCount = 1;
            actualArgsCount += std::count_if(argsStr.begin(), argsStr.end(),
                                             [](char c) { return c == ' '; });
        }
    }

    // 2- Check if command has valid number of arguments
    bool isValidArgs = (actualArgsCount == expectedArgsCount);

    // 3- Check if command is valid in current game state
    bool isValidInState = validCommands[state].end()
        != std::find(validCommands[state].begin(), validCommands[state].end(),
                     command->getCommandType());

    bool isValidOverall = isValidSyntax && isValidArgs && isValidInState;

    if (print) {
        std::cout << "Validating command '" << command->getCommandText()
                  << "' in state '" << stateTypeToString(state) << "'\n"
                  << std::endl;

        std::cout << "Command assumed effect: " << command->getEffectText()
                  << std::endl;

        std::cout << "\n1- Valid syntax: " << (isValidSyntax ? "true" : "false")
                  << std::endl;

        std::cout << "2- Valid arguments: " << (isValidArgs ? "true" : "false")
                  << std::endl;

        std::cout << "3- Valid in current state: "
                  << (isValidInState ? "true" : "false") << "\n"
                  << std::endl;

        if (command->getCommandType() == CommandType::tournament) {
            std::cout << "Passed initial validation: "
                      << (isValidOverall ? "true" : "false") << std::endl
                      << std::endl;

            // Perform tournament-specific validation
            if (isValidOverall) {
                isValidOverall = validateTournament(command);
            }
        }

        std::cout << "Will command take effect? "
                  << (isValidOverall ? "true" : "false") << "\n"
                  << std::endl;
    } else {
        // If not printing, still need to validate tournament
        if (command->getCommandType() == CommandType::tournament
            && isValidOverall) {
            isValidOverall = validateTournament(command, false);
        }
    }

    return isValidOverall;
}

bool CommandProcessor::validateTournament(Command* command, bool print) {

    std::string cmdText = command->getCommandText();

    // Check format
    size_t mPos = cmdText.find("-M");
    size_t pPos = cmdText.find("-P");
    size_t gPos = cmdText.find("-G");
    size_t dPos = cmdText.find("-D");

    if (mPos == std::string::npos || pPos == std::string::npos
        || gPos == std::string::npos || dPos == std::string::npos) {
        if (print) {
            std::cout
                << "4- Valid format: false\n"
                   "   ERROR: Invalid tournament command format. Expected: "
                << "tournament -M map1,map2,... -P strat1,strat2,... "
                << "-G numgames -D maxturns" << std::endl;
        }
        command->saveEffect("Invalid tournament command format. Expected: "
                            "tournament -M map1,map2,... -P strat1,strat2,... "
                            "-G numgames -D maxturns");
        return false;
    }

    if (print) {
        std::cout << "4- Valid format: true" << std::endl;
    }

    // Parse parameters temporarily for validation
    std::string mapsStr = cmdText.substr(mPos + 2, pPos - mPos - 2);
    mapsStr.erase(0, mapsStr.find_first_not_of(" \t"));
    mapsStr.erase(mapsStr.find_last_not_of(" \t") + 1);
    std::vector<std::string> maps = splitString(mapsStr, ',');

    std::string stratStr = cmdText.substr(pPos + 2, gPos - pPos - 2);
    stratStr.erase(0, stratStr.find_first_not_of(" \t"));
    stratStr.erase(stratStr.find_last_not_of(" \t") + 1);
    std::vector<std::string> strategies = splitString(stratStr, ',');

    std::string gamesStr = cmdText.substr(gPos + 2, dPos - gPos - 2);
    gamesStr.erase(0, gamesStr.find_first_not_of(" \t"));
    gamesStr.erase(gamesStr.find_last_not_of(" \t") + 1);
    int numGames;
    try {
        numGames = std::stoi(gamesStr);
    } catch (...) {
        if (print) {
            std::cout << "5- Able to parse number of games: false\n"
                      << "   ERROR: Could not parse number of games"
                      << std::endl;
        }
        command->saveEffect("Invalid number of games");
        return false;
    }

    if (print) {
        std::cout << "5- Able to parse number of games: true" << std::endl;
    }

    std::string turnsStr = cmdText.substr(dPos + 2);
    turnsStr.erase(0, turnsStr.find_first_not_of(" \t"));
    turnsStr.erase(turnsStr.find_last_not_of(" \t") + 1);
    int maxTurns;
    try {
        maxTurns = std::stoi(turnsStr);
    } catch (...) {
        if (print) {
            std::cout << "6- Able to parse number of max turns: false\n"
                         "   ERROR: Could not parse max turns"
                      << std::endl;
        }
        command->saveEffect("Invalid max turns");
        return false;
    }

    if (print) {
        std::cout << "6- Able to parse number of max turns: true" << std::endl;
    }

    // Validate ranges
    bool isValid = true;

    if (maps.size() < 1 || maps.size() > 5) {
        if (print) {
            std::cout << "7- Valid number of maps: false\n"
                         "   ERROR: Invalid number of maps (must be 1-5, got "
                      << maps.size() << ")" << std::endl;
        }
        command->saveEffect("Invalid number of maps (must be 1-5)");
        isValid = false;
    } else if (print) {
        std::cout << "7- Valid number of maps: true" << std::endl;
    }

    if (strategies.size() < 2 || strategies.size() > 4) {
        if (print) {
            std::cout
                << "8- Valid number of strategies: false\n"
                   "   ERROR: Invalid number of strategies (must be 2-4, got "
                << strategies.size() << ")" << std::endl;
        }
        command->saveEffect("Invalid number of strategies (must be 2-4)");
        isValid = false;
    } else if (print) {
        std::cout << "8- Valid number of strategies: true" << std::endl;
    }

    if (numGames < 1 || numGames > 5) {
        if (print) {
            std::cout << "9- Valid number of games: false\n"
                         "   ERROR: Invalid number of games (must be 1-5, got "
                      << numGames << ")" << std::endl;
        }
        command->saveEffect("Invalid number of games (must be 1-5)");
        isValid = false;
    } else if (print) {
        std::cout << "9- Valid number of games: true" << std::endl;
    }

    if (maxTurns < 10 || maxTurns > 50) {
        if (print) {
            std::cout << "10- Valid number of max turns: false\n"
                         "   ERROR: Invalid max turns (must be 10-50, got "
                      << maxTurns << ")" << std::endl;
        }
        command->saveEffect("Invalid max turns (must be 10-50)");
        isValid = false;
    } else if (print) {
        std::cout << "10- Valid number of max turns: true\n" << std::endl;
    }

    return isValid;
}

Tournament CommandProcessor::prepareTournament(const std::string &cmdText) {
    Tournament tournament = {};
    tournament.numGames = 0;
    tournament.maxTurns = 0;

    size_t mPos = cmdText.find("-M");
    size_t pPos = cmdText.find("-P");
    size_t gPos = cmdText.find("-G");
    size_t dPos = cmdText.find("-D");

    // Parse maps
    std::string mapsStr = cmdText.substr(mPos + 2, pPos - mPos - 2);
    mapsStr.erase(0, mapsStr.find_first_not_of(" \t"));
    mapsStr.erase(mapsStr.find_last_not_of(" \t") + 1);
    tournament.maps = splitString(mapsStr, ',');

    // Parse strategies
    std::string stratStr = cmdText.substr(pPos + 2, gPos - pPos - 2);
    stratStr.erase(0, stratStr.find_first_not_of(" \t"));
    stratStr.erase(stratStr.find_last_not_of(" \t") + 1);
    tournament.strategies = splitString(stratStr, ',');

    // Parse number of games
    std::string gamesStr = cmdText.substr(gPos + 2, dPos - gPos - 2);
    gamesStr.erase(0, gamesStr.find_first_not_of(" \t"));
    gamesStr.erase(gamesStr.find_last_not_of(" \t") + 1);
    tournament.numGames = std::stoi(gamesStr);

    // Parse max turns
    std::string turnsStr = cmdText.substr(dPos + 2);
    turnsStr.erase(0, turnsStr.find_first_not_of(" \t"));
    turnsStr.erase(turnsStr.find_last_not_of(" \t") + 1);
    tournament.maxTurns = std::stoi(turnsStr);

    return tournament;
}

void CommandProcessor::saveCommand(Command* command) {
    commandsList.push_back(command);
    Notify(this);
}

std::ostream &operator<<(std::ostream &os,
                         const CommandProcessor &commandProcessor) {
    os << "Commands in the the command processor: \n" << std::endl;
    for (Command* command : commandProcessor.commandsList) {
        os << "\t" << *command << std::endl;
    }
    return os << "\n";
}

//-------------------FileCommandProcessorAdapter-------------------
FileCommandProcessorAdapter::FileCommandProcessorAdapter(
    const std::string &fname)
    : filename(fname) {
    filepath = std::filesystem::absolute(filename).string();
    fileStream.open(filepath);
    if (!fileStream.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

// Reads command from file (file mode)
Command* FileCommandProcessorAdapter::readCommand() {
    std::string input;
    if (std::getline(fileStream, input)) {
        if (input.empty()
            || input.find_first_not_of(" \t\n\r") == std::string::npos) {
            return readCommand();
        }
        std::cout << "Reading command from file: '" << input << "'"
                  << std::endl;
        Command* cmd = new Command(input);
        saveCommand(cmd);
        return cmd;
    }
    return nullptr;
}

std::ostream &operator<<(std::ostream &os,
                         const FileCommandProcessorAdapter &fcpa) {
    os << "FileCommandProcessorAdapter reading from file: "
       << fcpa.getFileName();
    os << "Commands in the the command processor: \n" << std::endl;
    for (Command* command : fcpa.commandsList) {
        os << "\t" << *command << std::endl;
    }
    return os;
}