#pragma once
#include "Map/MapLoader.h"
#include "Map/Map.h"
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

  void command(const std::string &command);
	/*
	 * COMMANDS:
	 * MANUAL - Display STARTUP/PLAY MANUAL
	 * STARTUP commands:
	 * LOADMAP <filename>
	 * ADDPLAYER <playername>
	 * START - starts the game if map is loaded and at least two players are
	 * added
	 *
	 * PLAY commands:
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
   * VIEWMAP
	 * VIEWPLAYERS
	 * VIEWORDERS
	 * MOVEORDERS
	 * REMOVEORDER <order index>
	 * EXECUTEORDERS
   * 
	 * QUIT (Available in both STARTUP and PLAY states)
	 */

	void startupGame();
	void viewstartupDetails() const;
  void addPlayer(const std::string &playerName);
	void removePlayer(const std::string &playerName);

  void assignTerritories();
  void assignReinforcement();
  void viewPlayers() const;

	void runGame();

	friend std::ostream &operator<<(std::ostream &output,
									const GameEngine &gameEngine);

	bool isGameOver() const;

  private:
	State* currentState;
	std::vector<Player> players; // array of players
	Player* currentPlayer;		 // player whose turn it is
  Map* currentMap;
	MapLoader* mapLoader;
	std::string currentMapPath; // path to the current map file
};
