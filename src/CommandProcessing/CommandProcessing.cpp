#include "CommandProcessing.h"

// ---------------- Command class implementation ----------------
Command::Command() = default;
Command::Command(const std::string &cmd) {
	commandText = new std::string(cmd);
	effectText = new std::string("");
}
Command::Command(const std::string &cmd, const std::string &effect) {
	commandText = new std::string(cmd);
	effectText = new std::string(effect);
}
Command::Command(const Command &other) {
	commandText = new std::string(*(other.commandText));
	effectText = new std::string(*(other.effectText));
}
Command& Command::operator=(const Command &other) {
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

std::string& Command::getCommand() const {
	return *commandText;
}
std::string& Command::getEffect() const {
	return *effectText;
}
void Command::saveEffect(const std::string &effect) {
	*effectText = effect;

	Notify();
}
std::ostream& operator<<(std::ostream& out, const Command &command) {
	out << "Command: " << *(command.commandText) << "\nEffect: " << *(command.effectText);
	return out;
}

// ---------------- CommandProcessor class implementation ----------------

CommandProcessor::CommandProcessor() {
	_commandHistory = std::vector<Command*>();
};

CommandProcessor::CommandProcessor(const CommandProcessor &commandProcessor) {
	_commandHistory = std::vector<Command*>();
	for (const auto& cmd : commandProcessor._commandHistory) {
		_commandHistory.push_back(new Command(*cmd));
	}
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor &rhs) {
	if (this != &rhs) {
		for (auto& cmd : _commandHistory) {
			delete cmd;
		}
		_commandHistory.clear();
		for (const auto& cmd : rhs._commandHistory) {
			_commandHistory.push_back(new Command(*cmd));
		}
	}
	return *this;
}

CommandProcessor::~CommandProcessor() {
	for (auto& cmd : _commandHistory) {
		delete cmd;
	}
	_commandHistory.clear();
}

Command& CommandProcessor::getCommand() const {
	if (_commandHistory.empty()) {
		throw std::runtime_error("No commands in history.");
	}
	return *(_commandHistory.back());
}

void CommandProcessor::readCommand() {
	std::string input;
	std::cout << "Enter command: ";
	std::getline(std::cin, input);
	saveCommand(input);
}

void CommandProcessor::saveCommand(const std::string& command) {
    Command* newCommand = new Command(command);
    _commandHistory.push_back(newCommand);

    Notify();
}

bool CommandProcessor::validate(Command& command, StateType gameState) {
	const std::string& cmdText = command.getCommand();
	switch (gameState) {
		// TODO: Implement actual validation logic based on game state
	}
}

std::ostream& operator<<(std::ostream& os, const CommandProcessor& commandProcessor) {
    os << "Commands in the the command processor: \n" << std::endl;
    for (Command* command : commandProcessor._commandHistory) {
        os << "\t" << *command << std::endl;
    }
    return os << "\n";
}