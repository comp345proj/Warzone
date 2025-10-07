#pragma once
#include "Map/Map.h"
#include "Map/MapLoader.h"
#include "Utils/Utils.h"
#include <Player/Player.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

// forward declarations
class State;
class GameEngine;

//----------------------------State-------------------------------

class State {
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

  private:
	std::string state;
	/* State Name possibilites:
	 * Start
	 * MapLoaded
	 * MapValidated
	 * PlayersAdded
	 * AssignReinforcement
	 * Win
	 * Exit
	 *  - IssueOrders (maybe?)
	 *  - ExecuteOrders (maybe?)
	 */
	std::string currentPlayerTurn; // name of the player whose turn it is
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
  public:
	GameEngine();
	GameEngine(const GameEngine &other);			// Copy constructor
	GameEngine &operator=(const GameEngine &other); // Assignment operator
	~GameEngine();

	void command(const std::string &command);
	/*
	 * COMMANDS:
	 * STATE
	 * MANUAL
	 * LOADMAP <filename>
	 * VALIDATEMAP
	 * ADDPLAYER <playername>
	 * GAMESTART
	 * REPLAY
	 * QUIT
	 */

	std::string getState();
	void setState(const std::string &newState);
	void addPlayer(const std::string &playerName);

	friend std::ostream &operator<<(std::ostream &output,
									const GameEngine &gameEngine);

	bool isGameOver() const;

  private:
	State* state;
	std::vector<Player> players;
	Player* currentPlayer;
	Map* currentMap;
	MapLoader* mapLoader;
	std::string currentMapPath;
};
