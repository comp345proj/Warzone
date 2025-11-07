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

std::string &Command::getCommand() const {
	return *commandText;
}

std::string &Command::getEffect() const {
	return *effectText;
}

std::string Command::stringToLog() {
	return "Command issued: " + getCommand() + " | Effect: " + getEffect();
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

// ---------------- CommandProcessor class implementation ----------------

CommandProcessor::CommandProcessor() {
	_commandHistory = std::vector<Command*>();
};

CommandProcessor::CommandProcessor(const CommandProcessor &commandProcessor) {
	_commandHistory = std::vector<Command*>();
	for (const auto &cmd : commandProcessor._commandHistory) {
		_commandHistory.push_back(new Command(*cmd));
	}
}

CommandProcessor &CommandProcessor::operator=(const CommandProcessor &rhs) {
	if (this != &rhs) {
		for (auto &cmd : _commandHistory) {
			delete cmd;
		}
		_commandHistory.clear();
		for (const auto &cmd : rhs._commandHistory) {
			_commandHistory.push_back(new Command(*cmd));
		}
	}
	return *this;
}

CommandProcessor::~CommandProcessor() {
	for (auto &cmd : _commandHistory) {
		delete cmd;
	}
	_commandHistory.clear();
}

Command &CommandProcessor::getCommand() const {
	if (_commandHistory.empty()) {
		throw std::runtime_error("No commands in history.");
	}
	return *(_commandHistory.back());
}

// Get latest command for logging
std::string CommandProcessor::stringToLog() {
	return "CommandProcessor cmd: " + getCommand().stringToLog();
}

void CommandProcessor::readCommand() {
	std::string input;
	std::cout << "Enter command: ";
	std::getline(std::cin, input);
	saveCommand(input);
}

void CommandProcessor::saveCommand(const std::string &command) {
	Command* newCommand = new Command(command);
	_commandHistory.push_back(newCommand);

	Notify(this);
}

bool CommandProcessor::validate(Command &command, StateType gameState) {
	const std::string &cmdText = command.getCommand();

	auto setCommandEffect = [&](bool isValid, const std::string &stateName) {
		if (isValid) {
			command.saveEffect("Command '" + cmdText + "' is valid in " +
							   stateName + " state");
		} else {
			command.saveEffect("Command '" + cmdText + "' is invalid in " +
							   stateName + " state");
		}
		return isValid;
	};

	switch (gameState) {
	case StateType::START:
		return setCommandEffect(
			cmdText.find("loadmap") == 0 || cmdText == "quit", "START");

	case StateType::MAP_LOADED:
		return setCommandEffect(cmdText == "validatemap" ||
									cmdText.find("loadmap") == 0 ||
									cmdText == "quit",
								"MAP_LOADED");

	case StateType::MAP_VALIDATED:
		return setCommandEffect(cmdText.find("addplayer") == 0 ||
									cmdText.find("loadmap") == 0 ||
									cmdText == "quit",
								"MAP_VALIDATED");

	case StateType::PLAYERS_ADDED:
		return setCommandEffect(cmdText == "gamestart" || cmdText == "quit",
								"PLAYERS_ADDED");

	case StateType::ASSIGN_REINFORCEMENT:
		return setCommandEffect(cmdText == "issueorder" ||
									cmdText == "issueorders" ||
									cmdText == "quit",
								"ASSIGN_REINFORCEMENT");

	case StateType::ISSUE_ORDERS:
		return setCommandEffect(
			cmdText == "endissueorders" || cmdText == "quit", "ISSUE_ORDERS");

	case StateType::EXECUTE_ORDERS:
		return setCommandEffect(cmdText == "execorder" ||
									cmdText == "endexecorders" ||
									cmdText == "quit",
								"EXECUTE_ORDERS");

	case StateType::WIN:
		return setCommandEffect(cmdText == "play" || cmdText == "replay" ||
									cmdText == "quit",
								"WIN");

	default:
		command.saveEffect("Invalid game state");
		return false;
	}
}

std::ostream &operator<<(std::ostream &os,
						 const CommandProcessor &commandProcessor) {
	os << "Commands in the the command processor: \n" << std::endl;
	for (Command* command : commandProcessor._commandHistory) {
		os << "\t" << *command << std::endl;
	}
	return os << "\n";
}

// ---------------- FileLineReader class implementation ----------------
FileLineReader::FileLineReader() : _fileStream(nullptr) {}

FileLineReader::FileLineReader(const std::string &filePath) {
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

std::string &FileLineReader::readLineFromFile() {
	std::string line;

	if (_fileStream && std::getline(*_fileStream, line)) {
		return *(new std::string(line));
	}

	return *(new std::string("End of file"));
}

// ---------------- FileCommandProcessorAdapter class implementation
// ----------------
FileCommandProcessorAdapter::FileCommandProcessorAdapter()
	: CommandProcessor(), _fileLineReader(nullptr) {}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(
	const std::string &filePath)
	: _fileLineReader(new FileLineReader(filePath)) {}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(
	const FileCommandProcessorAdapter &fcp)
	: _fileLineReader(fcp._fileLineReader
						  ? new FileLineReader(*(fcp._fileLineReader))
						  : nullptr) {}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
	delete _fileLineReader;
}

FileCommandProcessorAdapter &
FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter &rhs) {
	if (this != &rhs) {
		delete _fileLineReader;
		_fileLineReader = rhs._fileLineReader
							  ? new FileLineReader(*(rhs._fileLineReader))
							  : nullptr;
	}
	return *this;
}

std::string &FileCommandProcessorAdapter::readCommand() {
	if (!_fileLineReader) {
		throw std::runtime_error("No file reader available");
	}

	try {
		std::string &command = _fileLineReader->readLineFromFile();
		saveCommand(command); // Save the command to history
		return command;
	} catch (const std::runtime_error &e) {
		throw std::runtime_error("Failed to read command from file: " +
								 std::string(e.what()));
	}
}