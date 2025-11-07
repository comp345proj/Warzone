#pragma once
#include "CommandProcessing/CommandProcessing.h"
#include "Map/Map.h"
#include "Map/MapLoader.h"
#include "Player/Player.h"
#include "Utils/Utils.h"
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Forward declarations
class State;
class GameEngine;

void printInvalidCommandError(StateType currentState);

//----------------------------State-------------------------------

class State {
  private:
	StateType state;
	std::string* currentPlayerTurn; // name of the player whose turn it is

  public:
	State(StateType state);
	State(const State &other);			  // Copy constructor
	State &operator=(const State &other); // Assignment operator
	~State();

	StateType getState() const;
	void setState(StateType newState);
	// void update(Command& command);

	std::string getCurrentPlayerTurn() const;
	void setCurrentPlayerTurn(const std::string &playerName);
	friend std::ostream &operator<<(std::ostream &out, const State &state);
};

//----------------------------GameEngine-------------------------------
class GameEngine : public ILoggable, public Subject {
  private:
	State* state;
	std::vector<Player*> players;
	Player* currentPlayer;
	Map* currentMap;
	MapLoader* mapLoader;
	std::string* currentMapPath;
	CommandProcessor* commandProcessor;
	Deck* deck;

  public:
	GameEngine();
	GameEngine(const GameEngine &other);			// Copy constructor
	GameEngine &operator=(const GameEngine &other); // Assignment operator
	GameEngine(CommandProcessor &cP);
	~GameEngine();

	void command(const std::string &command);
	void addPlayer(const std::string &playerName);
	bool isGameOver() const;

	CommandProcessor &getCommandProcessor();
	// void update(Command& command);

	// Logging method
	std::string stringToLog() override;

	StateType getState();
	void setState(StateType newState);

	friend std::ostream &operator<<(std::ostream &output,
									const GameEngine &gameEngine);

	// Static member for valid commands
	static std::map<StateType, std::vector<CommandType>> validCommands;

	// Static method to initialize valid commands
	// static void initializeValidCommands();

	// Static method to get valid commands for a state
	static const std::vector<CommandType> &
	getValidCommandsForState(StateType state);

	// Startup phase method
	void startupPhase();

	// Helper methods for startup phase
	void loadMap(const std::string &filename);
	void validateMap();
	void gameStart();
	void distributeInitialArmies();
	void drawInitialCards();
};