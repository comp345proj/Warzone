#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <Map/MapLoader.h>
#include <Player/Player.h>
#include "Map/MapLoader.h"
#include "Utils/Utils.h"

//forward declarations
class State;
class GameEngine;

//----------------------------State-------------------------------

class State {
  public:
	State(const std::string &name);
	State(const State &other);			  // Copy constructor
	State &operator=(const State &other); // Assignment operator
	~State();

	std::string getName() const;

  private:
	std::string name;
	/* State Name possibilites:
	 * SETUP (MapLoaded, MapValidated, PlayersAdded)
	 * ASSIGNREINFORCEMENTS
	 * ISSUEORDERS
	 * EXECUTEORDERS
	 * WIN
	 * END
	 */
  std::string currentPlayerTurn; // name of the player whose turn it is
};

//---------------------------GameEngine-------------------------------

class GameEngine {
  public:
	GameEngine();
	GameEngine(const GameEngine &other);			// Copy constructor
	GameEngine &operator=(const GameEngine &other); // Assignment operator
	~GameEngine();

  void setupGame();
  void viewSetupDetails() const;
  void runGame();

	void command(const std::string &command);
	/*
	 * Possible commands:
	 * MANUAL - Display this manual
	 * startup commands:
	 * LOADMAP <filename>
	 * ADDPLAYER <playername>
   * START - starts the game if map is loaded and at least two players are added
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

	std::string getCurrentStateName() const;

	bool isGameOver() const;

  private:
	State* currentState;
  std::vector<Player> players; // array of players
  Player* currentPlayer; // player whose turn it is
  MapLoader* currentMap;
  std::string currentMapPath; // path to the current map file
};
