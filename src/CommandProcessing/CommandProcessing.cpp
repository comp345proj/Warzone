#include "CommandProcessing.h"
#include <iostream>
#include <map>

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

std::string Command::stringToLog() {
	return "Command issued: " + this->getCommand() + " | Effect: " + this->getEffect();
}

void Command::saveEffect(const std::string &effect) {
	*effectText = effect;

	Notify(this);
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

// Get latest command for logging
std::string CommandProcessor::stringToLog() {
	return "CommandProcessor cmd: " + this->getCommand().stringToLog();
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

    Notify(this);
}

enum class CommandType {
	LOAD_MAP,
	VALIDATE_MAP,
	ADD_PLAYER,
	ASSIGN_COUNTRIES,
	ISSUE_ORDER,
	END_ISSUE_ORDERS,
	EXECUTE_ORDER,
	END_EXECUTE_ORDERS,
	WIN,
	END,
	PLAY,
};

enum class StateType {
	START,
	MAP_LOADED,
	MAP_VALIDATED,
	PLAYERS_ADDED,
	ASSIGN_REINFORCEMENT,
	ISSUE_ORDERS,
	EXECUTE_ORDERS,
	WIN,
};

std::map<StateType, std::vector<CommandType>> validCommands = {
	{StateType::START, {CommandType::LOAD_MAP}},

	{StateType::MAP_LOADED,
		{CommandType::LOAD_MAP, CommandType::VALIDATE_MAP}},

	{StateType::MAP_VALIDATED, {CommandType::ADD_PLAYER}},

	{StateType::PLAYERS_ADDED,
		{CommandType::ADD_PLAYER, CommandType::ASSIGN_COUNTRIES}},

	{StateType::ASSIGN_REINFORCEMENT, {CommandType::ISSUE_ORDER}},

	{StateType::ISSUE_ORDERS,
		{CommandType::ISSUE_ORDER, CommandType::END_ISSUE_ORDERS}},

	{StateType::EXECUTE_ORDERS,
		{CommandType::EXECUTE_ORDER, CommandType::END_EXECUTE_ORDERS}},

	{StateType::WIN, {CommandType::PLAY, CommandType::END}},
};

bool CommandProcessor::validate(Command& command, StateType gameState) {
	const std::string& cmdText = command.getCommand();
	switch (gameState) {
		case StateType::START:
			return (cmdText == "LOAD_MAP" || cmdText == "QUIT");
		case StateType::MAP_LOADED:
			return (cmdText == "VALIDATE_MAP" || cmdText == "LOAD_MAP" || cmdText == "QUIT");
		case StateType::MAP_VALIDATED:
			return (cmdText == "ADD_PLAYER" || cmdText == "LOAD_MAP" || cmdText == "QUIT");
		case StateType::PLAYERS_ADDED:
			return (cmdText == "ASSIGN_COUNTRIES" || cmdText == "LOAD_MAP" || cmdText == "QUIT");
		case StateType::ASSIGN_REINFORCEMENT:
			return (cmdText == "ISSUE_ORDERS" || cmdText == "END_REINFORCEMENT" || cmdText == "QUIT");
		case StateType::ISSUE_ORDERS:
			return (cmdText == "END_ISSUE_ORDERS" || cmdText == "QUIT");
		case StateType::EXECUTE_ORDERS:
			return (cmdText == "END_EXECUTE_ORDERS" || cmdText == "QUIT");
		case StateType::WIN:
			return (cmdText == "PLAY" || cmdText == "END");
		default:
			return false;
	}
}

std::ostream& operator<<(std::ostream& os, const CommandProcessor& commandProcessor) {
    os << "Commands in the the command processor: \n" << std::endl;
    for (Command* command : commandProcessor._commandHistory) {
        os << "\t" << *command << std::endl;
    }
    return os << "\n";
}

// ---------------- FileLineReader class implementation ----------------
FileLineReader::FileLineReader() : _fileStream(nullptr) {}

FileLineReader::FileLineReader(const std::string& filePath) {
	_fileStream = new std::ifstream(filePath);
	if (!_fileStream->is_open()) {
		throw std::runtime_error("Could not open file: " + filePath);
	}
}

FileLineReader::~FileLineReader() {
	if (_fileStream) {
		_fileStream->close();
		delete _fileStream;
	}
}

std::string& FileLineReader::readLineFromFile(){
	std::string line;

	if (_fileStream && std::getline(*_fileStream, line)) {
		return *(new std::string(line));
	} 
	
	return *(new std::string("End of file"));
}

// ---------------- FileCommandProcessorAdapter class implementation ----------------
FileCommandProcessorAdapter::FileCommandProcessorAdapter() 
	: CommandProcessor(), _fileLineReader(nullptr) {}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const std::string& filePath) : _fileLineReader(new FileLineReader(filePath)) {}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& fcp) : _fileLineReader(fcp._fileLineReader ? new FileLineReader(*(fcp._fileLineReader)) : nullptr) {}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete _fileLineReader;
}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter &rhs) {
    if (this != &rhs) {
        delete _fileLineReader;
        _fileLineReader = rhs._fileLineReader ? new FileLineReader(*(rhs._fileLineReader)) : nullptr;
    }
    return *this;
}

std::string& FileCommandProcessorAdapter::readCommand() {
	if (_fileLineReader) {
		return _fileLineReader->readLineFromFile();
	} else {
		return *(new std::string("No file reader available"));
	}
}