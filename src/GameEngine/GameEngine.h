#pragma once
#include "Map/MapLoader.h"
#include "Utils/Utils.h"
#include <Map/MapLoader.h>
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

	std::string getSubState() const;
  void setSubState(const std::string &newSubState);

	std::string getCurrentPlayerTurn() const;
	void setCurrentPlayerTurn(const std::string &playerName);

  private:
	std::string state;
	/* State Name possibilites:
	 * STARTUP (MapLoaded, MapValidated, PlayersAdded)
	 * PLAY
	 * (substates of PLAY)
	 *  - Reinforcement
	 *  - IssueOrders
	 *  - ExecuteOrders
	 *  - WIN
	 * END
	 */
	std::string subState;		   // name of the substate if any
	std::string currentPlayerTurn; // name of the player whose turn it is
};

//---------------------------GameEngine-------------------------------

class GameEngine {
  public:
	GameEngine();
	GameEngine(const GameEngine &other);			// Copy constructor
	GameEngine &operator=(const GameEngine &other); // Assignment operator
	~GameEngine();

	void startupGame();
	void viewstartupDetails() const;
	void runGame();

	void command(const std::string &command);
	/*
	 * Possible commands:
	 * MANUAL - Display this manual
	 * startup commands:
	 * LOADMAP <filename>
	 * ADDPLAYER <playername>
	 * START - starts the game if map is loaded and at least two players are
	 * added
	 *
	 * play commands:
	 * STARTDEPLOYMENT <# of armies> <target territory>
	 * DEPLOY <# of armies> <source territory> <target territory>
	 * ATTACK <# of armies> <source territory> <target territory>
	 * USECARD <card type> <location>
	 * ex// USECARD BOMB <territory name>
	 * ex// USECARD REINFORCEMENT <territory name>
	 * ex// USECARD BLOCKADE <territory name>
	 * ex// USECARD AIRLIFT <source territory> <target territory>
	 * ex// USECARD DIPLOMACY <player name>
	 * DRAWCARD
	 * VIEWHAND
	 * VIEWORDERS
	 * MOVEORDERS
	 * REMOVEORDER <order index>
	 * EXECUTEORDERS
	 * QUIT
	 * SHOWMAP
	 * SHOWPLAYERS
	 */

	void changeState(State* newState);

	void addPlayer(const std::string &playerName);
	void removePlayer(const std::string &playerName);
	void viewPlayers() const;

	friend std::ostream &operator<<(std::ostream &output,
									const GameEngine &gameEngine);

	bool isGameOver() const;

  private:
	State* currentState;
	std::vector<Player> players; // array of players
	Player* currentPlayer;		 // player whose turn it is
	MapLoader* currentMap;
	std::string currentMapPath; // path to the current map file
};
