#pragma once
#include <string>

namespace GameTypes {

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

enum class CommandType {
	LOAD_MAP,
	VALIDATE_MAP,
	ADD_PLAYER,
	GAME_START,
	ASSIGN_COUNTRIES,
	ISSUE_ORDER,
	END_ISSUE_ORDERS,
	EXECUTE_ORDER,
	END_EXECUTE_ORDERS,
	WIN,
	END,
	PLAY,
};

// Helper functions for string conversion
std::string stateTypeToString(StateType state);
std::string commandTypeToString(CommandType command);
CommandType stringToCommandType(const std::string &commandStr);
StateType stringToStateType(const std::string &stateStr);

} // namespace GameTypes

using namespace GameTypes;