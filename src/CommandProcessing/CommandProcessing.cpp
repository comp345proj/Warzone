#include "CommandProcessing.h"

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
void Command::setEffect(const std::string &effect) {
	*effectText = effect;

	Notify();
}
std::ostream& operator<<(std::ostream& out, const Command &command) {
	out << "Command: " << *(command.commandText) << "\nEffect: " << *(command.effectText);
	return out;
}
