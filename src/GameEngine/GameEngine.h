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

void printInvalidCommandError();

// Represents a game's state
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

    std::string getCurrentPlayerTurn() const;
    void setCurrentPlayerTurn(const std::string &playerName);
    friend std::ostream &operator<<(std::ostream &os, const State &state);
};

// Represents the whole game engine
class GameEngine : public ILoggable, public Subject {
  private:
    State* state;
    MapLoader* mapLoader;
    std::string* currentMapPath;
    Map* currentMap;
    Player* currentPlayer;
    Deck* deck;
    CommandProcessor* commandProcessor;
    LogObserver* logObserver;
    std::vector<Player*> players;

  public:
    explicit GameEngine(CommandProcessor* cmdProcessor);
    GameEngine(const GameEngine &other);
    GameEngine &operator=(const GameEngine &other);
    virtual ~GameEngine();

    // Command methods
    void startupPhase(bool runMainLoop = true);
    void loadMap(const std::string &filename);
    void validateMap();
    void addPlayer(const std::string &playerName);
    void gameStart(bool runMainLoop = true);
    void replay();

    // Main phases
    void mainGameLoop(bool runExecuteOrdersPhase = true);
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();

    // Helper methods
    int calculateReinforcement(Player* player);
    void distributeInitialArmies();
    void drawInitialCards();
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

    friend std::ostream &operator<<(std::ostream &os,
                                    const GameEngine &gameEngine);
};