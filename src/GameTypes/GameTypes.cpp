#include "GameTypes.h"

namespace GameTypes {

std::string stateTypeToString(StateType state) {
	switch (state) {
	case StateType::START:
		return "START";
	case StateType::MAP_LOADED:
		return "MAP_LOADED";
	case StateType::MAP_VALIDATED:
		return "MAP_VALIDATED";
	case StateType::PLAYERS_ADDED:
		return "PLAYERS_ADDED";
	case StateType::ASSIGN_REINFORCEMENT:
		return "ASSIGN_REINFORCEMENT";
	case StateType::ISSUE_ORDERS:
		return "ISSUE_ORDERS";
	case StateType::EXECUTE_ORDERS:
		return "EXECUTE_ORDERS";
	case StateType::WIN:
		return "WIN";
	default:
		return "UNKNOWN";
	}
}

StateType stringToStateType(const std::string &stateStr) {
	if (stateStr == "START")
		return StateType::START;
	if (stateStr == "MAP_LOADED")
		return StateType::MAP_LOADED;
	if (stateStr == "MAP_VALIDATED")
		return StateType::MAP_VALIDATED;
	if (stateStr == "PLAYERS_ADDED")
		return StateType::PLAYERS_ADDED;
	if (stateStr == "ASSIGN_REINFORCEMENT")
		return StateType::ASSIGN_REINFORCEMENT;
	if (stateStr == "ISSUE_ORDERS")
		return StateType::ISSUE_ORDERS;
	if (stateStr == "EXECUTE_ORDERS")
		return StateType::EXECUTE_ORDERS;
	if (stateStr == "WIN")
		return StateType::WIN;
	throw std::invalid_argument("Invalid state string: " + stateStr);
}

std::string commandTypeToString(CommandType command) {
	switch (command) {
	case CommandType::LOAD_MAP:
		return "LOAD_MAP";
	case CommandType::VALIDATE_MAP:
		return "VALIDATE_MAP";
	case CommandType::ADD_PLAYER:
		return "ADD_PLAYER";
	case CommandType::GAME_START:
		return "GAME_START";
	case CommandType::ASSIGN_COUNTRIES:
		return "ASSIGN_COUNTRIES";
	case CommandType::ISSUE_ORDER:
		return "ISSUE_ORDER";
	case CommandType::END_ISSUE_ORDERS:
		return "END_ISSUE_ORDERS";
	case CommandType::EXECUTE_ORDER:
		return "EXECUTE_ORDER";
	case CommandType::END_EXECUTE_ORDERS:
		return "END_EXECUTE_ORDERS";
	case CommandType::WIN:
		return "WIN";
	case CommandType::END:
		return "END";
	case CommandType::PLAY:
		return "PLAY";
	default:
		return "UNKNOWN";
	}
}

CommandType stringToCommandType(const std::string &commandStr) {
	if (commandStr == "LOAD_MAP")
		return CommandType::LOAD_MAP;
	if (commandStr == "VALIDATE_MAP")
		return CommandType::VALIDATE_MAP;
	if (commandStr == "ADD_PLAYER")
		return CommandType::ADD_PLAYER;
	if (commandStr == "GAME_START")
		return CommandType::GAME_START;
	if (commandStr == "ASSIGN_COUNTRIES")
		return CommandType::ASSIGN_COUNTRIES;
	if (commandStr == "ISSUE_ORDER")
		return CommandType::ISSUE_ORDER;
	if (commandStr == "END_ISSUE_ORDERS")
		return CommandType::END_ISSUE_ORDERS;
	if (commandStr == "EXECUTE_ORDER")
		return CommandType::EXECUTE_ORDER;
	if (commandStr == "END_EXECUTE_ORDERS")
		return CommandType::END_EXECUTE_ORDERS;
	if (commandStr == "WIN")
		return CommandType::WIN;
	if (commandStr == "END")
		return CommandType::END;
	if (commandStr == "PLAY")
		return CommandType::PLAY;
	throw std::invalid_argument("Invalid command string: " + commandStr);
}

} // namespace GameTypes