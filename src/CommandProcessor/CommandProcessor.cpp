#include "CommandProcessor.h"
#include <iostream>
#include <map>
#include <algorithm>

//---------------------------Command-------------------------------
Command::Command() = default;

Command::Command(const std::string &cmd, const std::string &effect)
	: commandText(new std::string(cmd)), effectText(new std::string(effect)) {}

Command::Command(const Command &other)
	: commandText(new std::string(*(other.commandText))),
	  effectText(new std::string(*(other.effectText))) {}

Command &Command::operator=(const Command &other) {
	if (this != &other) {
		delete commandText;
		delete effectText;
		commandText = new std::string(*(other.commandText));
		effectText = new std::string(*(other.effectText));
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
	Command* command = readCommand();
	return command;
}

std::vector<Command*>* CommandProcessor::getCommandsList() {
	return &_commandsList;
}

std::string CommandProcessor::stringToLog() {
	return "Latest Command: " + (_commandsList.empty()
									 ? "No commands processed yet."
									 : _commandsList.back()->stringToLog());
}

Command* CommandProcessor::readCommand() {
    std::string input;
    std::cout << "Enter command: ";
    std::getline(std::cin, input);
    if(input.empty()) {
        std::getline(std::cin, input);
    }
    Command* cmd = new Command(input);
    saveCommand(cmd);
    return cmd;
}

bool CommandProcessor::validate(std::string cmdText, StateType state) {
    // Extract just the command part (first word)
    std::string cmdPart = cmdText.substr(0, cmdText.find(' '));
    return validCommands[state].end() !=
           std::find(validCommands[state].begin(), validCommands[state].end(),
                     stringToCommandType(cmdPart));
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
	fileStream.open(filename);
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
		if (input.empty() ||
			input.find_first_not_of(" \t\n\r") == std::string::npos) {
			return readCommand();
		}

		Command* cmd = new Command(input);
		if (validate(input, StateType::start)) {
			cmd->saveEffect("Valid command from file: " + filename);
			saveCommand(cmd);
			return cmd;
		} else {
			cmd->saveEffect("Invalid command from file: " + filename);
			saveCommand(cmd);
			return nullptr;
		}
	}
	return nullptr;
}
