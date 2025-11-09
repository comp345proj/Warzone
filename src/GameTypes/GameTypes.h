#pragma once
#include <string>
#include <map>

namespace GameTypes {

enum class StateType {
	start,
	maploaded,
	mapvalidated,
	playeradded,
	assignreinforcement,
	issueorders,
	executeorders,
	win,
	invalid
};

enum class CommandType {
	loadmap,
	validatemap,
	addplayer,
	gamestart,
	replay,
	quit,
	invalid
};

enum class CardType {
	REINFORCEMENT,
	BOMB,
	AIRLIFT,
	BLOCKADE,
	DIPLOMACY,
	UNKNOWN
};

extern std::map<StateType, std::vector<CommandType>> validCommands;

extern std::map<CommandType, std::string> commandEffect;

// Helper functions for string conversion
std::string stateTypeToString(StateType state);
StateType stringToStateType(const std::string &stateStr);
std::string commandTypeToString(CommandType command);
CommandType stringToCommandType(const std::string &commandStr);
std::string cardTypeToString(CardType type);
CardType stringToCardType(const std::string &cardStr);

std::string getCommandArgsStr(CommandType command);
int getCommandArgsCount(CommandType command);

}

using namespace GameTypes;