#pragma once
#include <map>
#include <string>
#include <vector>

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

enum class CardType {
    REINFORCEMENT,
    BOMB,
    AIRLIFT,
    BLOCKADE,
    DIPLOMACY,
    UNKNOWN
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

std::string stateTypeToString(StateType state);
StateType stringToStateType(const std::string &stateStr);

std::string cardTypeToString(CardType type);
CardType stringToCardType(const std::string &cardStr);

std::string commandTypeToString(CommandType command);
CommandType stringToCommandType(const std::string &commandStr);
std::string getCommandArgsString(CommandType command);
int getCommandArgsCount(CommandType command);
extern std::map<StateType, std::vector<CommandType>> validCommands;
extern std::map<CommandType, std::string> commandEffect;

} // namespace GameTypes

using namespace GameTypes;