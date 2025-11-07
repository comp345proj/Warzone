#include "GameEngine.h"
#include <algorithm>
using std::cin;

void printInvalidCommandError(StateType currentState) {
	std::cout << "Invalid command. Current state: "
			  << stateTypeToString(currentState) << std::endl;
	std::cout << "Available commands in current state:" << std::endl;
	for (const auto &cmd : GameEngine::validCommands[currentState]) {
		std::cout << "  - " << commandTypeToString(cmd) << std::endl;
	}
}

//---------------------------State-------------------------------

// Constructors, destructor, copy constructor, assignment operator
State::State(StateType state) : state(state) {}
State::State(const State &other)
	: state(other.state), currentPlayerTurn(other.currentPlayerTurn) {}

State &State::operator=(const State &other) {
	if (this != &other) {
		state = other.state;
		currentPlayerTurn = other.currentPlayerTurn;
	}
	return *this;
}

State::~State() {
	delete currentPlayerTurn;
}

// Getters and setters
StateType State::getState() const {
	return state;
}

void State::setState(StateType newState) {
	state = newState;
}

std::string State::getCurrentPlayerTurn() const {
	return *currentPlayerTurn;
}

void State::setCurrentPlayerTurn(const std::string &playerName) {
	*currentPlayerTurn = playerName;
}

//---------------------------GameEngine-------------------------------

// Initialize static member
std::map<StateType, std::vector<CommandType>> GameEngine::validCommands = {
	{StateType::START, {CommandType::LOAD_MAP}},

	{StateType::MAP_LOADED, {CommandType::LOAD_MAP, CommandType::VALIDATE_MAP}},

	{StateType::MAP_VALIDATED, {CommandType::ADD_PLAYER}},

	{StateType::PLAYERS_ADDED,
	 {CommandType::ADD_PLAYER, CommandType::GAME_START,
	  CommandType::ASSIGN_COUNTRIES}},

	{StateType::ASSIGN_REINFORCEMENT, {CommandType::ISSUE_ORDER}},

	{StateType::ISSUE_ORDERS,
	 {CommandType::ISSUE_ORDER, CommandType::END_ISSUE_ORDERS}},

	{StateType::EXECUTE_ORDERS,
	 {CommandType::EXECUTE_ORDER, CommandType::END_EXECUTE_ORDERS}},

	{StateType::WIN, {CommandType::PLAY, CommandType::END}},
};

const std::vector<CommandType> &
GameEngine::getValidCommandsForState(StateType state) {
	// if (validCommands.empty()) {
	//     initializeValidCommands();
	// }
	return validCommands[state];
}

// -------------------------------

// Constructors, destructor, copy constructor, assignment operator
GameEngine::GameEngine()
	: state(new State(StateType::START)), currentMap(nullptr),
	  mapLoader(nullptr), currentMapPath(new std::string()),
	  currentPlayer(nullptr), deck(new Deck()) {
	// if (validCommands.empty()) {
	//     initializeValidCommands();
	// }
}

GameEngine::GameEngine(CommandProcessor &cP) : commandProcessor(&cP) {
	cP.validCommands = &validCommands;
};

GameEngine::GameEngine(const GameEngine &other)
	: state(new State(*other.state)), players(other.players),
	  currentPlayer(other.currentPlayer),
	  currentMap(new Map(*other.currentMap)),
	  mapLoader(new MapLoader(*other.mapLoader)), deck(new Deck(*other.deck)) {}

GameEngine &GameEngine::operator=(const GameEngine &other) {
	if (this != &other) {
		delete state;
		delete currentMap;
		delete deck;
		state = new State(*other.state);
		players = other.players;
		currentPlayer = other.currentPlayer;
		currentMap = new Map(*other.currentMap);
		mapLoader = new MapLoader(*other.mapLoader);
		deck = new Deck(*other.deck);
	}
	return *this;
}

GameEngine::~GameEngine() {
	delete state;
	delete currentMap;
	delete deck;
	players.clear();
	currentPlayer = nullptr;
}

// Getters and setters
StateType GameEngine::getState() {
	return state->getState();
}
void GameEngine::setState(StateType newState) {
	state->setState(newState);
}

CommandProcessor &GameEngine::getCommandProcessor() {
	return *commandProcessor;
}

/// @brief Primitive state function to handle user input
/// @param command
void GameEngine::command(const std::string &command) {
	std::vector<std::string> splitCommand = splitString(command, ' ');
	if (splitCommand.empty()) {
		printInvalidCommandError(state->getState());
		return;
	}

	// Convert command to uppercase for case-insensitive comparison
	std::string cmd = splitCommand[0];
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

	// Convert command string to CommandType
	CommandType commandType;
	if (cmd == "LOAD_MAP")
		commandType = CommandType::LOAD_MAP;
	else if (cmd == "VALIDATE_MAP")
		commandType = CommandType::VALIDATE_MAP;
	else if (cmd == "ADD_PLAYER")
		commandType = CommandType::ADD_PLAYER;
	else if (cmd == "ASSIGN_COUNTRIES")
		commandType = CommandType::ASSIGN_COUNTRIES;
	else if (cmd == "ISSUE_ORDER")
		commandType = CommandType::ISSUE_ORDER;
	else if (cmd == "EXECUTE_ORDER")
		commandType = CommandType::EXECUTE_ORDER;
	else if (cmd == "END_ISSUE_ORDERS")
		commandType = CommandType::END_ISSUE_ORDERS;
	else if (cmd == "END_EXECUTE_ORDERS")
		commandType = CommandType::END_EXECUTE_ORDERS;
	else if (cmd == "WIN")
		commandType = CommandType::WIN;
	else if (cmd == "PLAY")
		commandType = CommandType::PLAY;
	else if (cmd == "END")
		commandType = CommandType::END;
	else {
		printInvalidCommandError(state->getState());
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
				state->setState(StateType::MAP_LOADED);
				std::cout << "Map loaded successfully: " << mapName
						  << std::endl;
			} else {
				std::cout << "Failed to load map: " << mapName << std::endl;
			}
		} catch (const std::exception &e) {
			std::cout << "Error loading map: " << e.what() << std::endl;
		}
		break;
	}

	case CommandType::VALIDATE_MAP: {
		if (currentMap != nullptr && currentMap->validate()) {
			state->setState(StateType::MAP_VALIDATED);
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
			state->setState(StateType::PLAYERS_ADDED);
		}
		break;
	}

	case CommandType::ASSIGN_COUNTRIES: {
		if (players.empty() || currentMap == nullptr) {
			std::cout << "Cannot assign countries: No players or map not loaded"
					  << std::endl;
			break;
		}

		// Get all territories from the map
		// Get all territories from map
		std::vector<Territory*> territories;
		for (Continent* continent : currentMap->getContinents()) {
			std::vector<Territory*> continentTerritories =
				continent->getTerritories();
			territories.insert(territories.end(), continentTerritories.begin(),
							   continentTerritories.end());
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
			player->addTerritory(territory); // Takes Territory pointer
		}

		state->setState(StateType::ASSIGN_REINFORCEMENT);
		std::cout << "Countries assigned successfully" << std::endl;
		break;
	}

	case CommandType::ISSUE_ORDER: {
		// Assuming current player is set
		if (currentPlayer == nullptr) {
			currentPlayer = players[0]; // Start with first player
		}

		// Get a card from player's hand and issue order
		Hand* playerHand = currentPlayer->getHand();
		if (playerHand != nullptr && !playerHand->getCards().empty()) {
			Card* card = playerHand->getCards().front(); // Get first card
			currentPlayer->issueOrder(card,
									  nullptr); // Pass nullptr as deck for now
		} else {
			std::cout << "Player " << currentPlayer->getName()
					  << " has no cards to play" << std::endl;
			Card* card = deck->draw();
			if (card) {
				currentPlayer->addCard(card);
			}
			std::cout << "Playing a random card " << *card << " for "
					  << currentPlayer->getName() << std::endl;
			currentPlayer->issueOrder(card,
									  deck); // Pass nullptr as deck for now
		}

		// Move to next player
		auto it = std::find(players.begin(), players.end(), currentPlayer);
		if (it != players.end() && ++it != players.end()) {
			currentPlayer = *it;
		}
		state->setState(StateType::ISSUE_ORDERS);
		break;
	}

	case CommandType::END_ISSUE_ORDERS: {
		state->setState(StateType::EXECUTE_ORDERS);
		currentPlayer = nullptr; // Reset current player for execution phase
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
		for (const auto &continent : currentMap->getContinents()) {
			totalTerritories += continent->getTerritories().size();
		}

		for (Player* player : players) {
			if (player->getTerritories().size() == totalTerritories) {
				state->setState(StateType::WIN);
				std::cout << "Player " << player->getName()
						  << " has won the game!" << std::endl;
				return;
			}
		}

		if (!ordersRemaining) {
			// If no more orders to execute, move back to reinforcement phase
			state->setState(StateType::ASSIGN_REINFORCEMENT);
		}
		break;
	}

	case CommandType::END_EXECUTE_ORDERS: {
		state->setState(StateType::ASSIGN_REINFORCEMENT);
		break;
	}

	case CommandType::WIN: {
		// Already in win state, waiting for PLAY or END command
		break;
	}

	// Play again
	case CommandType::PLAY: {
		// Reset the game state
		state->setState(StateType::START);
		players.clear();
		delete currentMap;
		currentMap = nullptr;
		delete mapLoader;
		mapLoader = nullptr;
		currentPlayer = nullptr;
		break;
	}

	case CommandType::END: {
		std::cout << "Thank you for playing!" << std::endl;
		exit(0);
	}
	}

	// Display available commands for current state
	std::cout << "\nAvailable commands in current state ("
			  << stateTypeToString(getState()) << "):" << std::endl;

	for (const auto &cmd : validCommands[getState()]) {
		std::cout << "  - " << commandTypeToString(cmd) << std::endl;
	}
}

/// @brief Function which checks if the game is over based on state
/// @return
bool GameEngine::isGameOver() const {
	return state->getState() == StateType::WIN;
}

// GameEngine logging method
std::string GameEngine::stringToLog() {
	return "GameEngine state changed to: " + stateTypeToString(getState());
}

// Adds a player to the game
void GameEngine::addPlayer(const std::string &playerName) {
	// Check if we already have 6 players
	if (players.size() >= 6) {
		std::cout << "Maximum number of players (6) reached" << std::endl;
		return;
	}

	Player* newPlayer = new Player(playerName);
	players.push_back(newPlayer);
	std::cout << "Player added: " << playerName << std::endl;

	// Move to PLAYERS_ADDED state if we have at least 2 players
	if (players.size() >= 2) {
		state->setState(StateType::PLAYERS_ADDED);
	}
}

// Implementation of helper methods
void GameEngine::loadMap(const std::string &filename) {
	if (state->getState() != StateType::START &&
		state->getState() != StateType::MAP_LOADED) {
		std::cout << "Cannot load map in current state" << std::endl;
		return;
	}

	std::filesystem::path currentPath = std::filesystem::current_path();
	std::filesystem::path mapPath = currentPath / "res" / filename;

	try {
		mapLoader = new MapLoader(mapPath.string());
		currentMap = mapLoader->loadMap();
		if (currentMap != nullptr) {
			state->setState(StateType::MAP_LOADED);
			std::cout << "Map loaded successfully: " << filename << std::endl;
			Notify(this);
		} else {
			std::cout << "Failed to load map: " << filename << std::endl;
		}
	} catch (const std::exception &e) {
		std::cout << "Error loading map: " << e.what() << std::endl;
	}
}

void GameEngine::validateMap() {
	if (state->getState() != StateType::MAP_LOADED) {
		std::cout << "Must load a map before validating" << std::endl;
		return;
	}

	if (currentMap != nullptr && currentMap->validate()) {
		state->setState(StateType::MAP_VALIDATED);
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

void GameEngine::gameStart() {
	if (state->getState() != StateType::PLAYERS_ADDED) {
		std::cout << "Cannot start game in current state" << std::endl;
		return;
	}

	if (players.size() < 2 || players.size() > 6) {
		std::cout << "Need between 2 and 6 players to start the game"
				  << std::endl;
		return;
	}

	// 1. Fairly distribute territories
	std::vector<Territory*> allTerritories;
	for (Continent* continent : currentMap->getContinents()) {
		auto territories = continent->getTerritories();
		allTerritories.insert(allTerritories.end(), territories.begin(),
							  territories.end());
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
	std::cout << "Territories have been fairly distributed" << std::endl;

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
	state->setState(StateType::ASSIGN_REINFORCEMENT);
	std::cout << "\nGame has started! Moving to reinforcement phase."
			  << std::endl;
	Notify(this);
}

void GameEngine::startupPhase() {
	state->setState(StateType::START);
	std::cout << "=== Game Startup Phase ===" << std::endl;

	while (state->getState() != StateType::ASSIGN_REINFORCEMENT) {
		std::cout << "\nCurrent state: " << stateTypeToString(state->getState())
				  << std::endl;
		std::cout << "Available commands:" << std::endl;

		// Display available commands for current state
		for (const auto &cmd : validCommands[state->getState()]) {
			std::cout << "  - " << commandTypeToString(cmd) << std::endl;
		}

		std::cout << "\nEnter command: ";
		std::string input;
		std::getline(std::cin, input);

		// Split input into command and arguments
		std::vector<std::string> parts = splitString(input, ' ');
		if (parts.empty())
			continue;

		std::string cmd = parts[0];
		std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

		if (cmd == "LOAD_MAP" && parts.size() > 1) {
			loadMap(parts[1]);
		} else if (cmd == "VALIDATE_MAP") {
			validateMap();
		} else if (cmd == "ADD_PLAYER" && parts.size() > 1) {
			addPlayer(parts[1]);
		} else if (cmd == "GAME_START") {
			gameStart();
		} else {
			std::cout << "Invalid command. Please try again." << std::endl;
		}
	}
}