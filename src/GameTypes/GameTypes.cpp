#include "GameTypes.h"

namespace GameTypes {

//---------------------------StateType-------------------------------
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

//---------------------------CardType--------------------------------
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

//---------------------------CommandType-----------------------------
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
    if (command == CommandType::tournament)
        return "tournament";
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
    if (commandStr == "tournament")
        return CommandType::tournament;
    return CommandType::invalid;
}

std::string getCommandArgsString(CommandType command) {
    if (command == CommandType::loadmap)
        return "filename";
    if (command == CommandType::validatemap)
        return "";
    if (command == CommandType::addplayer)
        return "playername";
    if (command == CommandType::gamestart)
        return "";
    if (command == CommandType::replay)
        return "";
    if (command == CommandType::quit)
        return "";
    if (command == CommandType::tournament)
        return "-M maplist -P strategylist -G numgames -D maxturn";
    return "invalid";
}

int getCommandArgsCount(CommandType command) {
    if (command == CommandType::loadmap)
        return 1;
    if (command == CommandType::validatemap)
        return 0;
    if (command == CommandType::addplayer)
        return 1;
    if (command == CommandType::gamestart)
        return 0;
    if (command == CommandType::replay)
        return 0;
    if (command == CommandType::quit)
        return 0;
    if (command == CommandType::tournament)
        return 8;
    return -1;
}

std::map<StateType, std::vector<CommandType>> validCommands = {
    {StateType::start, {CommandType::loadmap, CommandType::tournament}},

    {StateType::maploaded, {CommandType::loadmap, CommandType::validatemap}},

    {StateType::mapvalidated, {CommandType::addplayer}},

    {StateType::playeradded, {CommandType::addplayer, CommandType::gamestart}},

    {StateType::win, {CommandType::replay, CommandType::quit}}};

std::map<CommandType, std::string> commandEffect = {
    {CommandType::loadmap, "Loads the map."},
    {CommandType::validatemap, "Validates the map."},
    {CommandType::addplayer, "Adds a player."},
    {CommandType::gamestart, "Starts the game."},
    {CommandType::replay, "Replays the game."},
    {CommandType::quit, "Exits the game."},
    {CommandType::tournament, "Starts tournament mode."},
    {CommandType::invalid, "Invalid command."}};

} // namespace GameTypes