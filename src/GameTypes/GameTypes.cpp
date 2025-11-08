#include "GameTypes.h"

namespace GameTypes {

std::string stateTypeToString(StateType state) {
	if (state == StateType::start)
		return "start";
	if (state == StateType::maploaded)
		return "maploaded";
	if (state == StateType::mapvalidated)
		return "mapvalidated";
	if (state == StateType::playeradded)
		return "playeradded";
	if (state == StateType::assignreinforcement)
		return "assignreinforcement";
	if (state == StateType::issueorders)
		return "issueorders";
	if (state == StateType::executeorders)
		return "executeorders";
	if (state == StateType::win)
		return "win";
	return "invalid";
}

StateType stringToStateType(const std::string &stateStr) {
	if (stateStr == "start")
		return StateType::start;
	if (stateStr == "maploaded")
		return StateType::maploaded;
	if (stateStr == "mapvalidated")
		return StateType::mapvalidated;
	if (stateStr == "playeradded")
		return StateType::playeradded;
	if (stateStr == "assignreinforcement")
		return StateType::assignreinforcement;
	if (stateStr == "issueorders")
		return StateType::issueorders;
	if (stateStr == "executeorders")
		return StateType::executeorders;
	if (stateStr == "win")
		return StateType::win;
	return StateType::invalid;
}

std::string commandTypeToString(CommandType command) {
	if (command == CommandType::loadmap)
		return "loadmap";
	if (command == CommandType::validatemap)
		return "validatemap";
	if (command == CommandType::addplayer)
		return "addplayer";
	if (command == CommandType::gamestart)
		return "gamestart";
	if (command == CommandType::replay)
		return "replay";
	if (command == CommandType::quit)
		return "quit";
	return "invalid";
}

CommandType stringToCommandType(const std::string &commandStr) {
	if (commandStr == "loadmap")
		return CommandType::loadmap;
	if (commandStr == "validatemap")
		return CommandType::validatemap;
	if (commandStr == "addplayer")
		return CommandType::addplayer;
	if (commandStr == "gamestart")
		return CommandType::gamestart;
	if (commandStr == "replay")
		return CommandType::replay;
	if (commandStr == "quit")
		return CommandType::quit;
	return CommandType::invalid;
}

std::string getCommandArgs(CommandType command) {
	switch (command) {
		case CommandType::loadmap:
			return "filename";
		case CommandType::addplayer:
			return "playername";
		default:
			return "";
	}
}

std::string cardTypeToString(CardType type) {
	if (type == CardType::REINFORCEMENT)
		return "Reinforcement";
	if (type == CardType::BOMB)
		return "Bomb";
	if (type == CardType::BLOCKADE)
		return "Blockade";
	if (type == CardType::AIRLIFT)
		return "Airlift";
	if (type == CardType::DIPLOMACY)
		return "Diplomacy";
	return "Unknown";
}

CardType stringToCardType(const std::string &cardStr) {
	if (cardStr == "Reinforcement")
		return CardType::REINFORCEMENT;
	if (cardStr == "Blockade")
		return CardType::BLOCKADE;
	if (cardStr == "Bomb")
		return CardType::BOMB;
	if (cardStr == "Airlift")
		return CardType::AIRLIFT;
	if (cardStr == "Diplomacy")
		return CardType::DIPLOMACY;
	return CardType::UNKNOWN;
}

std::map<StateType, std::vector<CommandType>> validCommands = {
	{StateType::start, {CommandType::loadmap}},

	{StateType::maploaded, {CommandType::loadmap, CommandType::validatemap}},

	{StateType::mapvalidated, {CommandType::addplayer}},

	{StateType::playeradded, {CommandType::addplayer, CommandType::gamestart}},

	{StateType::win, {CommandType::replay, CommandType::quit}}
};

} // namespace GameTypes