#pragma once
#include "Map/Map.h"
#include "Map/MapLoader.h"
#include "Utils/Utils.h"
#include <Player/Player.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <map>
// forward declarations
class State;
class GameEngine;


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
	// START,
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



std::string commandTypeToString(CommandType state);
void printInvalidCommandError();
void printInvalidCommandError(StateType currentState, CommandType commandType,
	std::map<StateType, std::vector<CommandType>> validCommands);

std::string stateTypeToString(StateType state);

//----------------------------State-------------------------------

class State {
  private:
	std::string* state;
	std::string* currentPlayerTurn; // name of the player whose turn it is

  public:
	State(const std::string &state);
	State(const std::string &state, const std::string &subState);
	State(const State &other);			  // Copy constructor
	State &operator=(const State &other); // Assignment operator
	~State();

	std::string getState() const;
	void setState(const std::string &newState);

	std::string getCurrentPlayerTurn() const;
	void setCurrentPlayerTurn(const std::string &playerName);
	friend std::ostream &operator<<(std::ostream &out, const State &state);
};

/*--StateTransitionLogic-----------------------------
 * LOADMAP <filename> - valid in: (Start, MapLoaded) - transitions to:
 * (MapLoaded) VALIDATEMAP - valid in: (MapLoaded) - transitions to:
 * (MapValidated) ADDPLAYER <playername> - valid in: (PlayersAdded) -
 * transitions to: (PlayersAdded) START - valid in: (PlayersAdded) - transitions
 * to: (AssignReinforcement) REPLAY - valid in: (Win) - transitions to: (Start)
 * QUIT - valid in: (any state) - transitions to: (Exit)
 */

//---------------------------GameEngine-------------------------------

class GameEngine {
  private:
	State* state;
	std::vector<Player*> players;
	Player* currentPlayer;
	Map* currentMap;
	MapLoader* mapLoader;
	std::string* currentMapPath;
	std::map<StateType, std::vector<CommandType>> validCommands;

  public:
	GameEngine();
	GameEngine(const GameEngine &other);			// Copy constructor
	GameEngine &operator=(const GameEngine &other); // Assignment operator
	~GameEngine();

	void command(const std::string &command);
    void addPlayer(const std::string &playerName);
    bool isGameOver() const;

	std::string getState();
	void setState(const std::string &newState);

	friend std::ostream &operator<<(std::ostream &output,
									const GameEngine &gameEngine);


	std::map<StateType, std::vector<CommandType>> getValidCommands() const;
	std::map<StateType, std::vector<CommandType>> setValidCommands() const;
};