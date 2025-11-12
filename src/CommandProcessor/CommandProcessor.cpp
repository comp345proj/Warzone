#include "CommandProcessor.h"
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
            actualArgsCount += std::count_if(argsStr.begin(),
                                             argsStr.end(),
                                             [](char c) { return c == ' '; });
        }
    }

    // 2- Check if command has valid number of arguments
    bool isValidArgs = (actualArgsCount == expectedArgsCount);

    // 3- Check if command is valid in current game state
    bool isValidInState = validCommands[state].end()
        != std::find(validCommands[state].begin(),
                     validCommands[state].end(),
                     command->getCommandType());

    bool isValidOverall = isValidSyntax && isValidArgs && isValidInState;

    if (print) {
        std::cout << "Validating command '" << command->getCommandText()
                  << "' in state '" << stateTypeToString(state) << "'\n"
                  << std::endl;

        std::cout << "Command assumed effect: " << command->getEffectText()
                  << std::endl;

        std::cout << "\n1- Valid syntax: " << isValidSyntax << std::endl;

        std::cout << "2- Valid arguments: " << isValidArgs << std::endl;

        std::cout << "3- Valid in current state: " << isValidInState << "\n"
                  << std::endl;

        std::cout << "Will command take effect? " << isValidOverall << "\n"
                  << std::endl;
    }

    return isValidOverall;
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