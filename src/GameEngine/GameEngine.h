#pragma once
#include "CommandProcessor/CommandProcessor.h"
#include "LoggingObserver/LoggingObserver.h"
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

// Function to demonstrate the main game loop
void testMainGameLoop();

void printInvalidCommandError();

//----------------------------State-------------------------------

class State {
  private:
    StateType stateType;
    std::string* currentPlayerTurn;

  public:
    State(StateType type);
    State(const State &other);
    State &operator=(const State &other);
    ~State();

    StateType getStateType() const;
    void setStateType(StateType newType);
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
    LogObserver* logObserver;

  public:
    explicit GameEngine(CommandProcessor* cmdProcessor);
    GameEngine(const GameEngine &other);
    GameEngine &operator=(const GameEngine &other);
    ~GameEngine();

    // Command methods
    void startupPhase();
    void loadMap(const std::string &filename);
    void validateMap();
    void addPlayer(const std::string &playerName);
    void distributeInitialArmies();
    void drawInitialCards();
    void gameStart();

    // Main phases
    void mainGameLoop();
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();

    // Helper methods
    int calculateReinforcement(Player* player);
    bool checkWinCondition();
    void removeDefeatedPlayers();
    bool isGameOver() const;

    // Logging method
    std::string stringToLog() override;

    // Getters for private members
    const std::vector<Player*> &getPlayers() const;
    Player* getCurrentPlayer() const;
    Map* getCurrentMap() const;
    MapLoader* getMapLoader() const;
    Deck* getDeck() const;
    LogObserver* getLogObserver() const;
    CommandProcessor &getCommandProcessor();
    StateType getState();
    void setState(StateType newState);
    static const std::vector<CommandType> &
    getValidCommandsForState(StateType state);

    friend std::ostream &operator<<(std::ostream &output,
                                    const GameEngine &gameEngine);
};