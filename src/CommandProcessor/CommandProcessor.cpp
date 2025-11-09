#include "CommandProcessor.h"
#include <algorithm>
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

    commandText = new std::string(trimmedCmd);
    type = stringToCommandType(commandText->substr(0, commandText->find(' ')));
    effectText = new std::string(commandEffect[type]);
}

Command::Command(const Command &other)
    : commandText(new std::string(*(other.commandText))),
      effectText(new std::string(*(other.effectText))), type(other.type) {}

Command &Command::operator=(const Command &other) {
    if (this != &other) {
        delete commandText;
        delete effectText;
        commandText = new std::string(*(other.commandText));
        effectText = new std::string(*(other.effectText));
        type = other.type;
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

CommandType Command::getType() const {
    return type;
}

std::string Command::stringToLog() {
    return "Command: " + getCommandText() + " | Effect: " + getEffectText();
}

void Command::saveEffect(const std::string &effect) {
    *effectText = effect;
    Notify(this);
}

std::ostream &operator<<(std::ostream &out, const Command &command) {
    out << "Command: " << *(command.commandText)
        << "\nEffect: " << *(command.effectText);
    return out;
}

//---------------------------CommandProcessor----------------------
CommandProcessor::CommandProcessor() {
    _commandsList = std::vector<Command*>();
};

CommandProcessor::CommandProcessor(const CommandProcessor &commandProcessor) {
    _commandsList = std::vector<Command*>();
    for (const auto &cmd : commandProcessor._commandsList) {
        _commandsList.push_back(new Command(*cmd));
    }
}

CommandProcessor &CommandProcessor::operator=(const CommandProcessor &rhs) {
    if (this != &rhs) {
        for (auto &cmd : _commandsList) {
            delete cmd;
        }
        _commandsList.clear();
        for (const auto &cmd : rhs._commandsList) {
            _commandsList.push_back(new Command(*cmd));
        }
    }
    return *this;
}

CommandProcessor::~CommandProcessor() {
    for (auto &cmd : _commandsList) {
        delete cmd;
    }
    _commandsList.clear();
}

Command* CommandProcessor::getCommand() {
    return readCommand();
}

std::vector<Command*>* CommandProcessor::getCommandsList() {
    return &_commandsList;
}

std::string CommandProcessor::stringToLog() {
    return "Latest Command: "
        + (_commandsList.empty() ? "No commands processed yet."
                                 : _commandsList.back()->stringToLog());
}

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

    bool isValidSyntax = command->getType() != CommandType::invalid;

    size_t expectedArgsCount = getCommandArgsCount(command->getType());
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

    bool isValidArgs = (actualArgsCount == expectedArgsCount);

    bool isValidInState = validCommands[state].end()
        != std::find(validCommands[state].begin(),
                     validCommands[state].end(),
                     command->getType());

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
    _commandsList.push_back(command);
    Notify(this);
}

std::ostream &operator<<(std::ostream &os,
                         const CommandProcessor &commandProcessor) {
    os << "Commands in the the command processor: \n" << std::endl;
    for (Command* command : commandProcessor._commandsList) {
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

Command* FileCommandProcessorAdapter::readCommand() {
    std::string input;
    if (std::getline(fileStream, input)) {
        if (input.empty()
            || input.find_first_not_of(" \t\n\r") == std::string::npos) {
            return readCommand();
        }
        Command* cmd = new Command(input);
        saveCommand(cmd);
        return cmd;
    }
    return nullptr;
}
