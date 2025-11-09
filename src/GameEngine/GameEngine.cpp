#include "GameEngine.h"
#include "Map/Map.h"
#include <algorithm>
using std::cin;

void printInvalidCommandError() {
    std::cout << "Invalid command." << std::endl;
}

//---------------------------State-------------------------------
State::State(StateType stateType) : stateType(stateType) {}
State::State(const State &other)
    : stateType(other.stateType), currentPlayerTurn(other.currentPlayerTurn) {}

State &State::operator=(const State &other) {
    if (this != &other) {
        stateType = other.stateType;
        currentPlayerTurn = other.currentPlayerTurn;
    }
    return *this;
}

State::~State() {
    delete currentPlayerTurn;
}

// Getters and setters
StateType State::getStateType() const {
    return stateType;
}

void State::setStateType(StateType newStateType) {
    stateType = newStateType;
}

std::string State::getCurrentPlayerTurn() const {
    return *currentPlayerTurn;
}

void State::setCurrentPlayerTurn(const std::string &playerName) {
    *currentPlayerTurn = playerName;
}

//---------------------------GameEngine-------------------------------
GameEngine::GameEngine(CommandProcessor* cmdProcessor)
    : state(new State(StateType::start)), currentMap(nullptr),
      mapLoader(nullptr), currentMapPath(new std::string()),
      currentPlayer(nullptr), deck(new Deck()), commandProcessor(cmdProcessor),
      logObserver(new LogObserver()) {}

GameEngine::GameEngine(const GameEngine &other)
    : state(new State(*other.state)), players(other.players),
      currentPlayer(other.currentPlayer),
      currentMap(new Map(*other.currentMap)),
      mapLoader(new MapLoader(*other.mapLoader)), deck(new Deck(*other.deck)),
      commandProcessor(new CommandProcessor(*other.commandProcessor)),
      logObserver(new LogObserver()) {}

GameEngine &GameEngine::operator=(const GameEngine &other) {
    if (this != &other) {
        delete state;
        delete currentMap;
        delete deck;
        delete logObserver;
        state = new State(*other.state);
        players = other.players;
        currentPlayer = other.currentPlayer;
        currentMap = new Map(*other.currentMap);
        mapLoader = new MapLoader(*other.mapLoader);
        deck = new Deck(*other.deck);
        commandProcessor = new CommandProcessor(*other.commandProcessor);
        logObserver = new LogObserver();
    }
    return *this;
}

GameEngine::~GameEngine() {
    delete state;
    delete currentMap;
    delete deck;
    delete logObserver;
    players.clear();
    currentPlayer = nullptr;
}

void GameEngine::startupPhase(bool runMainLoop) {
    std::cout << "\nStarting Warzone Game" << std::endl;
    std::cout << "===================" << std::endl;

    // Attach LogObserver to GameEngine and CommandProcessor
    this->Attach(logObserver);
    commandProcessor->Attach(logObserver);

    while (state->getStateType() != StateType::win) {
        StateType currentState = state->getStateType();
        std::cout << "\nCurrent state: " << stateTypeToString(currentState)
                  << std::endl;
        std::cout << "Available commands:" << std::endl;
        for (const auto &cmd : validCommands[currentState]) {
            std::cout << "  - " << commandTypeToString(cmd) << " "
                      << getCommandArgsStr(cmd) << std::endl;
        }

        Command* cmd = commandProcessor->getCommand();
        if (cmd == nullptr) {
            std::cout << "\nEnd of command file reached. Exiting game."
                      << std::endl;
            state->setStateType(StateType::win);
            break;
        }

        std::string cmdText = cmd->getCommandText();

        // Validate command for current state
        if (!commandProcessor->validate(cmd, currentState, false)) {
            printInvalidCommandError();
            continue;
        }

        // Process valid command
        try {
            std::string cmdPart = cmdText.substr(0, cmdText.find(' '));
            CommandType cmdType = stringToCommandType(cmdPart);
            switch (cmdType) {
                case CommandType::loadmap: {
                    size_t spacePos = cmdText.find(" ");
                    if (spacePos != std::string::npos) {
                        std::string filename = cmdText.substr(spacePos + 1);
                        loadMap(filename);
                        cmd->saveEffect("Map '" + filename + "' processed");
                    } else {
                        cmd->saveEffect(
                            "Invalid loadmap command: missing filename");
                    }
                    break;
                }

                case CommandType::validatemap:
                    validateMap();
                    cmd->saveEffect("Map validation processed");
                    break;

                case CommandType::addplayer: {
                    size_t spacePos = cmdText.find(" ");
                    if (spacePos != std::string::npos) {
                        std::string playerName = cmdText.substr(spacePos + 1);
                        addPlayer(playerName);
                        cmd->saveEffect("Player '" + playerName
                                        + "' processed");
                    } else {
                        cmd->saveEffect(
                            "Invalid addplayer command: missing player name");
                    }
                    break;
                }

                case CommandType::gamestart:
                    gameStart(runMainLoop);
                    cmd->saveEffect("Game start processed");
                    break;

                case CommandType::quit:
                    state->setStateType(StateType::win);
                    cmd->saveEffect("Game ended");
                    break;

                default:
                    cmd->saveEffect("Command not implemented");
                    break;
            }
        } catch (const std::exception &e) {
            cmd->saveEffect("Error executing command: "
                            + std::string(e.what()));
        }
    }

    std::cout << "\nGame ended." << std::endl;
}

void GameEngine::loadMap(const std::string &filename) {
    if (state->getStateType() != StateType::start
        && state->getStateType() != StateType::maploaded) {
        std::cout << "Cannot load map in current state" << std::endl;
        return;
    }

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path mapPath = currentPath / "res" / filename;

    try {
        mapLoader = new MapLoader(mapPath.string());
        currentMap = mapLoader->loadMap();
        if (currentMap != nullptr) {
            state->setStateType(StateType::maploaded);
            std::cout << "Map loaded successfully: " << filename << std::endl;
            Notify(this);
        } else {
            std::cout << "Failed to load map: " << filename << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "Error loading map: " << e.what() << std::endl;
    }
}

void GameEngine::addPlayer(const std::string &playerName) {
    // Check if we already have 6 players
    if (players.size() >= 6) {
        std::cout << "Maximum number of players (6) reached" << std::endl;
        return;
    }

    Player* newPlayer = new Player(playerName);
    players.push_back(newPlayer);

    // Attach LogObserver to the player's OrdersList
    newPlayer->getOrdersList()->Attach(logObserver);

    std::cout << "Player added: " << playerName << std::endl;

    // Move to playeradded state if we have at least 2 players
    if (players.size() >= 2) {
        state->setStateType(StateType::playeradded);
    }
}

void GameEngine::validateMap() {
    if (state->getStateType() != StateType::maploaded) {
        std::cout << "Must load a map before validating" << std::endl;
        return;
    }

    if (currentMap != nullptr && currentMap->validate()) {
        state->setStateType(StateType::mapvalidated);
        std::cout << "Map validated successfully" << std::endl;
        Notify(this);
    } else {
        std::cout << "Map validation failed" << std::endl;
    }
}

void GameEngine::distributeInitialArmies() {
    // Give each player 50 initial army units
    for (Player* player : players) {
        player->setReinforcementPool(50);
    }
    std::cout << "Each player received 50 initial army units" << std::endl;
}

void GameEngine::drawInitialCards() {

    // Each player draws 2 cards
    for (Player* player : players) {
        for (int i = 0; i < 2; i++) {
            Card* card = deck->draw();
            if (card) {
                player->addCard(card);
            }
        }
    }
    std::cout << "Each player has drawn 2 initial cards" << std::endl;
}

void GameEngine::gameStart(bool runMainLoop) {
    if (state->getStateType() != StateType::playeradded) {
        std::cout << "Cannot start game in current state" << std::endl;
        return;
    }

    if (players.size() < 2 || players.size() > 6) {
        std::cout << "Need between 2 and 6 players to start the game"
                  << std::endl;
        return;
    }

    // Print all continents and their details first
    std::cout << "\n=== Map Continents ===" << std::endl;
    for (Continent* continent : currentMap->getContinents()) {
        std::cout << "Continent Name: " << continent->getName()
                  << ", Territories: " << continent->getTerritories().size()
                  << ", Bonus: " << continent->getBonus() << std::endl;
    }

    // 1. Fairly distribute territories
    std::vector<Territory*> allTerritories;
    for (Continent* continent : currentMap->getContinents()) {
        auto territories = continent->getTerritories();
        allTerritories.insert(
            allTerritories.end(), territories.begin(), territories.end());
    }

    // Shuffle territories for random distribution
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(allTerritories.begin(), allTerritories.end(), g);

    // Distribute territories to players
    size_t playerCount = players.size();
    for (size_t i = 0; i < allTerritories.size(); ++i) {
        players[i % playerCount]->addTerritory(allTerritories[i]);
    }

    // Print each player's allocated territories
    std::cout << "\n=== Territory Distribution ===" << std::endl;
    for (Player* player : players) {
        std::cout << "\nPlayer " << player->getName()
                  << " received:" << std::endl;
        for (Territory* territory : player->getTerritories()) {
            std::cout << "  - " << territory->getName() << std::endl;
        }
    }

    std::cout << "\nTerritories have been fairly distributed" << std::endl;

    // 2. Randomly determine play order
    std::shuffle(players.begin(), players.end(), g);
    std::cout << "\nPlayer order has been randomized. New order:" << std::endl;
    for (size_t i = 0; i < players.size(); ++i) {
        std::cout << (i + 1) << ". " << players[i]->getName() << std::endl;
    }

    // 3. Give initial armies
    distributeInitialArmies();

    // 4. Draw initial cards
    drawInitialCards();

    // 5. Switch to play phase
    state->setStateType(StateType::assignreinforcement);
    std::cout << "\nGame has started! Moving to reinforcement phase."
              << std::endl;
    Notify(this);

    // Only call mainGameLoop if runMainLoop is true
    if (runMainLoop) {
        mainGameLoop();
    } else {
		state->setStateType(StateType::win);
	}
}

void GameEngine::mainGameLoop(bool runExecuteOrdersPhase) {
    while (!checkWinCondition()) {
        // Remove any defeated players before starting the next round
        removeDefeatedPlayers();

        // Check if we have a winner after removing defeated players
        if (checkWinCondition())
            break;

        // Execute each phase in order
        reinforcementPhase();
        issueOrdersPhase();
        if (runExecuteOrdersPhase) {
            executeOrdersPhase();
        } else {
            break;
        }
    }

    // Announce winner
    for (Player* player : players) {
        if (!player->getTerritories().empty()) {
            std::cout << "Player " << player->getName() << " has won the game!"
                      << std::endl;
            break;
        }
    }

    state->setStateType(StateType::win);
}

void GameEngine::reinforcementPhase() {
    std::cout << "\n=== Reinforcement Phase ===" << std::endl;

    for (Player* player : players) {
        int reinforcements = calculateReinforcement(player);
        player->setReinforcementPool(player->getReinforcementPool()
                                     + reinforcements);

        std::cout << "Player " << player->getName() << " receives "
                  << reinforcements << " reinforcement armies" << std::endl;
    }
}

void GameEngine::issueOrdersPhase() {
    std::cout << "\n=== Issue Orders Phase ===" << std::endl;

    // Round-robin: Each player completes ALL their order issuing before moving
    // to next player
    for (Player* player : players) {
        std::cout << "\n--- Player " << player->getName() << "'s Turn ---"
                  << std::endl;

        // Reset available reinforcement pool to match actual pool
        player->resetAvailableReinforcementPool();

        // Call issueOrder() which handles everything atomically
        // Player must finish all their actions before this returns
        player->issueOrder(deck);

        std::cout << "Player " << player->getName()
                  << " has finished their turn." << std::endl;
    }

    std::cout << "\nAll players have finished issuing orders." << std::endl;
}

void GameEngine::executeOrdersPhase() {
    std::cout << "\n=== Execute Orders Phase ===" << std::endl;

    // Reset conquest flags for all players at the start of execution phase
    for (Player* player : players) {
        player->setConqueredTerritoryThisTurn(false);
    }

    for (Player* player : players) {
        std::cout << "\n--- Executing orders for " << player->getName()
                  << " ---" << std::endl;

        OrdersList* ordersList = player->getOrdersList();

        // Phase 1: Execute all Deploy orders for this player
        std::cout << "Executing Deploy orders for " << player->getName()
                  << "..." << std::endl;
        auto orders = ordersList->getOrders(); // Get copy of list
        for (auto it = orders.begin(); it != orders.end();) {
            Order* order = *it;
            if (dynamic_cast<Deploy*>(order) != nullptr) {
                order->Attach(logObserver);
                order->execute();
                ordersList->remove(order);
                orders = ordersList->getOrders();
                it = orders.begin();
            } else {
                ++it;
            }
        }

        // Phase 2: Execute all other orders for this player
        std::cout << "Executing other orders for " << player->getName() << "..."
                  << std::endl;
        orders = ordersList->getOrders(); // Refresh list
        while (!orders.empty()) {
            Order* order = orders.front();
            order->Attach(logObserver);
            order->execute();
            ordersList->remove(order);
            orders = ordersList->getOrders(); // Refresh list
        }

        std::cout << "Player " << player->getName()
                  << " has completed all orders." << std::endl;
    }

    // Clear negotiated pairs at the end of the turn
    Advance::clearNegotiatedPairs();

    std::cout << "\n=== All orders have been executed ===" << std::endl;
}

int GameEngine::calculateReinforcement(Player* player) {
    int territoryCount = player->getTerritories().size();
    int baseReinforcement = std::max(3, territoryCount / 3);
    int continentBonus = 0;

    std::cout << "\nCalculating reinforcements for Player " << player->getName()
              << ":" << std::endl;
    std::cout << "  Territory count: " << territoryCount << std::endl;
    std::cout << "  Base reinforcement (max(3, territories/3)): "
              << baseReinforcement << std::endl;

    // Check for continent control bonuses
    for (Continent* continent : currentMap->getContinents()) {
        bool controlsContinent = true;
        for (Territory* territory : continent->getTerritories()) {
            // Check if territory is in player's territories
            bool found = false;
            for (Territory* playerTerritory : player->getTerritories()) {
                if (territory == playerTerritory) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                controlsContinent = false;
                break;
            }
        }
        if (controlsContinent) {
            continentBonus += continent->getBonus();
            std::cout << "  + Bonus " << continent->getBonus()
                      << " for controlling " << continent->getName()
                      << std::endl;
        }
    }

    std::cout << "  Base reinforcement: " << baseReinforcement << std::endl;
    std::cout << "  Total bonuses: " << continentBonus << std::endl;
    std::cout << "  Final reinforcement: "
              << (baseReinforcement + continentBonus) << std::endl;

    return baseReinforcement + continentBonus;
}

bool GameEngine::checkWinCondition() {
    if (players.empty())
        return false;

    size_t totalTerritories = 0;
    for (Continent* continent : currentMap->getContinents()) {
        totalTerritories += continent->getTerritories().size();
    }

    for (Player* player : players) {
        if (player->getTerritories().size() == totalTerritories) {
            return true;
        }
    }

    return false;
}

void GameEngine::removeDefeatedPlayers() {
    players.erase(std::remove_if(players.begin(),
                                 players.end(),
                                 [](Player* player) {
                                     if (player->getTerritories().empty()) {
                                         std::cout << "Player "
                                                   << player->getName()
                                                   << " has been eliminated!"
                                                   << std::endl;
                                         return true;
                                     }
                                     return false;
                                 }),
                  players.end());
}

bool GameEngine::isGameOver() const {
    return state->getStateType() == StateType::win;
}

std::string GameEngine::stringToLog() {
    return "GameEngine state changed to: " + stateTypeToString(getState());
}

CommandProcessor &GameEngine::getCommandProcessor() {
    return *commandProcessor;
}

const std::vector<Player*> &GameEngine::getPlayers() const {
    return players;
}

Player* GameEngine::getCurrentPlayer() const {
    return currentPlayer;
}

Map* GameEngine::getCurrentMap() const {
    return currentMap;
}

MapLoader* GameEngine::getMapLoader() const {
    return mapLoader;
}

LogObserver* GameEngine::getLogObserver() const {
    return logObserver;
}

StateType GameEngine::getState() {
    return state->getStateType();
}

void GameEngine::setState(StateType newState) {
    state->setStateType(newState);
    Notify(this);
}