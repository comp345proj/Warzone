#include "GameEngine.h"
#include <algorithm>
using std::cin;

std::string commandTypeToString(CommandType state)
{
	switch (state) {
	case CommandType::LOAD_MAP:
		return "LOAD_MAP";
	case CommandType::VALIDATE_MAP:
		return "VALIDATE_MAP";
	case CommandType::ADD_PLAYER:
		return "ADD_PLAYER";
	case CommandType::ASSIGN_COUNTRIES:
		return "ASSIGN_COUNTRIES";
	case CommandType::ISSUE_ORDER:
		return "ISSUE_ORDER";
	case CommandType::EXECUTE_ORDER:
		return "EXECUTE_ORDER";
	case CommandType::END_ISSUE_ORDERS:
		return "END_ISSUE_ORDERS";
	case CommandType::END_EXECUTE_ORDERS:
		return "END_EXECUTE_ORDERS";
	case CommandType::WIN:
		return "WIN";
	case CommandType::END:
		return "END";
	case CommandType::PLAY:
		return "PLAY";
	}
}

std::string stateTypeToString(StateType state)
{
	switch (state) {
	case StateType::START:
		return "START";
	case StateType::MAP_LOADED:
		return "MAP_LOADED";
	case StateType::MAP_VALIDATED:
		return "MAP_VALIDATED";
	case StateType::PLAYERS_ADDED:
		return "PLAYERS_ADDED";
	case StateType::ASSIGN_REINFORCEMENT:
		return "ASSIGN_REINFORCEMENT";
	case StateType::ISSUE_ORDERS:
		return "ISSUE_ORDERS";
	case StateType::EXECUTE_ORDERS:
		return "EXECUTE_ORDERS";
	case StateType::WIN:
		return "WIN";
	}
}

void printInvalidCommandError() {
	std::cout << "\nInvalid Command entered..." << std::endl;
}

void printInvalidCommandError(
	StateType currentState,
	CommandType commandType,
	std::map<StateType, std::vector<CommandType>> validCommands
) {
	const auto &validCommandsForState = validCommands[currentState];
	if (std::find(validCommandsForState.begin(), validCommandsForState.end(),
				  commandType) == validCommandsForState.end()) {
		std::cout << "\nInvalid command '" << commandTypeToString(commandType)
				  << "' for current state: " << stateTypeToString(currentState) << std::endl;

		// Display available commands
		std::cout << "Available commands in current state:" << std::endl;
		for (const auto &cmd : validCommandsForState) {
			std::cout << "  - " << commandTypeToString(cmd) << std::endl;
		}
	}
}

//---------------------------State-------------------------------

// Constructors, destructor, copy constructor, assignment operator
State::State(const std::string &state) : state(new std::string(state)), currentPlayerTurn(new std::string()) {}
State::State(const std::string &state, const std::string &subState)
	: state(new std::string(state)), currentPlayerTurn(new std::string(subState)) {}
State::State(const State &other)
	: state(new std::string(*other.state)), currentPlayerTurn(new std::string(*other.currentPlayerTurn)) {}

State &State::operator=(const State &other) {
	if (this != &other) {
		delete state;
		delete currentPlayerTurn;
		state = new std::string(*other.state);
		currentPlayerTurn = new std::string(*other.currentPlayerTurn);
	}
	return *this;
}

State::~State() {
	delete state;
	delete currentPlayerTurn;
}

// Getters and setters
std::string State::getState() const {
	return *state;
}

void State::setState(const std::string &newState) {
	*state = newState;
}

std::string State::getCurrentPlayerTurn() const {
	return *currentPlayerTurn;
}

void State::setCurrentPlayerTurn(const std::string &playerName) {
	*currentPlayerTurn = playerName;
}

//---------------------------GameEngine-------------------------------

// Constructors, destructor, copy constructor, assignment operator
GameEngine::GameEngine()
	: state(new State("START")), currentMap(nullptr), mapLoader(nullptr),
	  currentMapPath(new std::string()), currentPlayer(nullptr), validCommands({
		  {StateType::START, 
			{CommandType::LOAD_MAP}},
		  
			{StateType::MAP_LOADED, 
			{CommandType::LOAD_MAP, CommandType::VALIDATE_MAP}},
		  
			{StateType::MAP_VALIDATED, 
			{CommandType::ADD_PLAYER}},
		  
			{StateType::PLAYERS_ADDED, 
			{CommandType::ADD_PLAYER, CommandType::ASSIGN_COUNTRIES}},
		  
			{StateType::ASSIGN_REINFORCEMENT, 
			{CommandType::ISSUE_ORDER}},
		  
			{StateType::ISSUE_ORDERS, 
			{CommandType::ISSUE_ORDER, CommandType::END_ISSUE_ORDERS}},
		  
			{StateType::EXECUTE_ORDERS, 
			{CommandType::EXECUTE_ORDER, CommandType::END_EXECUTE_ORDERS, CommandType::WIN}},
		  
			{StateType::WIN, 
			{CommandType::PLAY, CommandType::END}},
	  }) {}

GameEngine::GameEngine(const GameEngine &other)
	: state(new State(*other.state)), players(other.players),
	  currentPlayer(other.currentPlayer),
	  currentMap(new Map(*other.currentMap)),
	  mapLoader(new MapLoader(*other.mapLoader)) {}

GameEngine &GameEngine::operator=(const GameEngine &other) {
	if (this != &other) {
		delete state;
		delete currentMap;
		state = new State(*other.state);
		players = other.players;
		currentPlayer = other.currentPlayer;
		currentMap = new Map(*other.currentMap);
		mapLoader = new MapLoader(*other.mapLoader);
	}
	return *this;
}

GameEngine::~GameEngine() {
	delete state;
	delete currentMap;
	getValidCommands().clear();
	players.clear();
	currentPlayer = nullptr;
}

// Getters and setters
std::string GameEngine::getState() {
	return state->getState();
}
void GameEngine::setState(const std::string &newState) {
	state->setState(newState);
}

/// @brief Primitive state function to handle user input
/// @param command
void GameEngine::command(const std::string &command) {
    std::vector<std::string> splitCommand = splitString(command, ' ');
	if (splitCommand.empty()) {
        printInvalidCommandError();
        return;
    }

    // Convert command to uppercase for case-insensitive comparison
    std::string cmd = splitCommand[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    // Convert current state string to StateType enum for validation
    StateType currentState;
    if (state->getState() == "START") currentState = StateType::START;
    else if (state->getState() == "MAP_LOADED") currentState = StateType::MAP_LOADED;
    else if (state->getState() == "MAP_VALIDATED") currentState = StateType::MAP_VALIDATED;
    else if (state->getState() == "PLAYERS_ADDED") currentState = StateType::PLAYERS_ADDED;
    else if (state->getState() == "ASSIGN_REINFORCEMENT") currentState = StateType::ASSIGN_REINFORCEMENT;
    else if (state->getState() == "ISSUE_ORDERS") currentState = StateType::ISSUE_ORDERS;
    else if (state->getState() == "EXECUTE_ORDERS") currentState = StateType::EXECUTE_ORDERS;
    else if (state->getState() == "WIN") currentState = StateType::WIN;
    else {
        printInvalidCommandError();
        return;
    }

    // Convert command string to CommandType
    CommandType commandType;
    if (cmd == "LOAD_MAP") commandType = CommandType::LOAD_MAP;
    else if (cmd == "VALIDATE_MAP") commandType = CommandType::VALIDATE_MAP;
    else if (cmd == "ADD_PLAYER") commandType = CommandType::ADD_PLAYER;
    else if (cmd == "ASSIGN_COUNTRIES") commandType = CommandType::ASSIGN_COUNTRIES;
    else if (cmd == "ISSUE_ORDER") commandType = CommandType::ISSUE_ORDER;
    else if (cmd == "EXECUTE_ORDER") commandType = CommandType::EXECUTE_ORDER;
    else if (cmd == "END_ISSUE_ORDERS") commandType = CommandType::END_ISSUE_ORDERS;
    else if (cmd == "END_EXECUTE_ORDERS") commandType = CommandType::END_EXECUTE_ORDERS;
    else if (cmd == "WIN") commandType = CommandType::WIN;
    else if (cmd == "PLAY") commandType = CommandType::PLAY;
    else if (cmd == "END") commandType = CommandType::END;
    else {
        printInvalidCommandError();
		// Check if command is valid for current state
		return;
    }

    // Handle each command type
    switch (commandType) {
        case CommandType::LOAD_MAP: {
            std::string mapName = "World.map"; // Default map
            if (splitCommand.size() > 1) {
                mapName = splitCommand[1];
            }
            
            std::filesystem::path currentPath = std::filesystem::current_path();
            std::filesystem::path mapPath = currentPath / "res" / mapName;
            
            try {
                mapLoader = new MapLoader(mapPath.string());
                currentMap = mapLoader->loadMap();
                if (currentMap != nullptr) {
                    state->setState("MAP_LOADED");
                    std::cout << "Map loaded successfully: " << mapName << std::endl;
                } else {
                    std::cout << "Failed to load map: " << mapName << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error loading map: " << e.what() << std::endl;
            }
            break;
        }

        case CommandType::VALIDATE_MAP: {
            if (currentMap != nullptr && currentMap->validate()) {
                state->setState("MAP_VALIDATED");
                std::cout << "Map validated successfully" << std::endl;
            } else {
                std::cout << "Map validation failed" << std::endl;
            }
            break;
        }

        case CommandType::ADD_PLAYER: {
            if (splitCommand.size() < 2) {
                std::cout << "Please provide a player name" << std::endl;
                break;
            }
            addPlayer(splitCommand[1]);
            
            // If we have added at least one player, move to PLAYERS_ADDED state
            if (players.size() > 0) {
                state->setState("PLAYERS_ADDED");
            }
            break;
        }

        case CommandType::ASSIGN_COUNTRIES: {
            if (players.empty() || currentMap == nullptr) {
                std::cout << "Cannot assign countries: No players or map not loaded" << std::endl;
                break;
            }

            // Get all territories from the map
            // Get all territories from map
            std::vector<Territory*> territories;
            for (Continent* continent : currentMap->getContinents()) {
                std::vector<Territory*> continentTerritories = continent->getTerritories();
                territories.insert(territories.end(), continentTerritories.begin(), continentTerritories.end());
            }
            
            if (territories.empty()) {
                std::cout << "No territories to assign" << std::endl;
                break;
            }

            // Randomly assign territories to players using modern C++ random
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(territories.begin(), territories.end(), g);
            
            size_t playerCount = players.size();
            for (size_t i = 0; i < territories.size(); ++i) {
                Player* player = players[i % playerCount];
                Territory* territory = territories[i];
                
                // Assign territory to player
                player->addTerritory(territory);  // Takes Territory pointer
            }

            state->setState("ASSIGN_REINFORCEMENT");
            std::cout << "Countries assigned successfully" << std::endl;
            break;
        }

        case CommandType::ISSUE_ORDER: {
            // Assuming current player is set
            if (currentPlayer == nullptr) {
                currentPlayer = players[0];  // Start with first player
            }

            // Get a card from player's hand and issue order
            Hand* playerHand = currentPlayer->getHand();
            if (playerHand != nullptr && !playerHand->getCards().empty()) {
                Card* card = playerHand->getCards().front();  // Get first card
                currentPlayer->issueOrder(card, nullptr);  // Pass nullptr as deck for now
            } else {
                std::cout << "Player " << currentPlayer->getName() << " has no cards to play" << std::endl;
            }
            
            // Move to next player
            auto it = std::find(players.begin(), players.end(), currentPlayer);
            if (it != players.end() && ++it != players.end()) {
                currentPlayer = *it;
            }
            break;
        }

        case CommandType::END_ISSUE_ORDERS: {
            state->setState("EXECUTE_ORDERS");
            currentPlayer = nullptr;  // Reset current player for execution phase
            break;
        }

        case CommandType::EXECUTE_ORDER: {
            bool ordersRemaining = false;
            for (Player* player : players) {
                OrdersList* orders = player->getOrdersList();
                if (orders != nullptr && !orders->getOrders().empty()) {
                    Order* order = orders->getOrders().front();
                    order->execute();
                    orders->remove(order);
                    ordersRemaining = true;
                }
            }
            
            // Check for win condition
            size_t totalTerritories = 0;
            for (const auto& continent : currentMap->getContinents()) {
                totalTerritories += continent->getTerritories().size();
            }
            
            for (Player* player : players) {
                if (player->getTerritories().size() == totalTerritories) {
                    state->setState("WIN");
                    std::cout << "Player " << player->getName() << " has won the game!" << std::endl;
                    return;
                }
            }
            
            if (!ordersRemaining) {
                // If no more orders to execute, move back to reinforcement phase
                state->setState("ASSIGN_REINFORCEMENT");
            }
            break;
        }

        case CommandType::END_EXECUTE_ORDERS: {
            state->setState("ASSIGN_REINFORCEMENT");
            break;
        }

        case CommandType::WIN: {
            // Already in win state, waiting for PLAY or END command
            break;
        }

		// Play again
        case CommandType::PLAY: {
            // Reset the game state
            state->setState("START");
            players.clear();
            delete currentMap;
            currentMap = nullptr;
            delete mapLoader;
            mapLoader = nullptr;
            currentPlayer = nullptr;
            break;
        }

        case CommandType::END: {
            state->setState("END");
            std::cout << "Thank you for playing!" << std::endl;
            break;
        }
    }

	// Display available commands for current state
	std::cout << "\nAvailable commands in current state ("
			  << stateTypeToString(currentState) << "):" << std::endl;

	for (const auto &cmd : validCommands[currentState]) {
		std::cout << "  - " << commandTypeToString(cmd) << std::endl;
	}
}

/// @brief Function which checks if the game is over based on state
/// @return
bool GameEngine::isGameOver() const {
	return state->getState() == "END";
}

std::map<StateType, std::vector<CommandType>> GameEngine::getValidCommands() const {
    return validCommands;
}

void GameEngine::addPlayer(const std::string& playerName) {
    Player* newPlayer = new Player(playerName);
    players.push_back(newPlayer);
	std::cout << "Player added: " << playerName << std::endl;
}