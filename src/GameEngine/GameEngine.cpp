#include "GameEngine.h"

//---------------------------State-------------------------------

State::State(const std::string &name) : name(name), currentPlayerTurn("") {}
State::State(const State &other)
	: name(other.name), currentPlayerTurn(other.currentPlayerTurn) {}

State &State::operator=(const State &other) {
	if (this != &other) {
		name = other.name;
		currentPlayerTurn = other.currentPlayerTurn;
	}
	return *this;
}

State::~State() = default;

std::string State::getName() const {
	return name;
}

//---------------------------GameEngine-------------------------------

GameEngine::GameEngine()
	: currentState(new State("Start")),
	  currentMap(new MapLoader("../../res/World.map")), currentPlayer(nullptr) {
}

GameEngine::GameEngine(const GameEngine &other)
  : currentState(new State(*other.currentState)),
    players(other.players), currentPlayer(other.currentPlayer),
    currentMap(new MapLoader(*other.currentMap)) {}

GameEngine &GameEngine::operator=(const GameEngine &other) {
  if (this != &other) {
    delete currentState;
    delete currentMap;
    currentState = new State(*other.currentState);
    players = other.players;
    currentPlayer = other.currentPlayer;
    currentMap = new MapLoader(*other.currentMap);
  }
  return *this;
}

GameEngine::~GameEngine() {
  delete currentState;
  delete currentMap;
  players.clear();
  currentPlayer = nullptr;
}

void GameEngine::command(const std::string &command) {
	if (currentState) {
		// split the command by space character and then store into an array
		std::vector<std::string> splitCommand = splitString(command, ' ');
		if (splitCommand[0] == "MANUAL") {
			std::cout << "Possible commands:\n"
					  << "startup commands:\n"
					  << "LOADMAP <filename>\n"
					  << "ADDPLAYER <playername>\n"
					  << "\nplay commands:\n"
					  << "STARTDEPLOYMENT <# of armies> <target territory>\n"
					  << "DEPLOY <# of armies> <source territory> <target "
						 "territory>\n"
					  << "ATTACK <# of armies> <source territory> <target "
						 "territory>\n"
					  << "USECARD <card type> <location>\n"
					  << "  ex// USECARD BOMB <territory name>\n"
					  << "  ex// USECARD REINFORCEMENT <territory name>\n"
					  << "  ex// USECARD BLOCKADE <territory name>\n"
					  << "  ex// USECARD AIRLIFT <source territory> <target "
						 "territory>\n"
					  << "  ex// USECARD DIPLOMACY <player name>\n"
					  << "DRAWCARD\n"
					  << "VIEWHAND\n"
					  << "VIEWORDERS\n"
					  << "MOVEORDERS\n"
					  << "REMOVEORDER <order index>\n"
					  << "EXECUTEORDERS\n"
					  << "QUIT\n"
					  << "SHOWMAP\n"
					  << "SHOWPLAYERS\n";
		} else if (splitCommand[0] == "LOADMAP") {
			// load map function call
			// validate map
			currentMap->loadMap();
			changeState(new State("MapValidated"));
			changeState(new State("PlayersAdded"));
		} else if (splitCommand[0] == "ADDPLAYER") {
			// add player function call
			addPlayer(splitCommand[1]);
		} else if (splitCommand[0] == "STARTDEPLOYMENT") {
			// deploy soldiers to given map territory upon start of turn
		} else if (splitCommand[0] == "DEPLOY") {
			// deploy soldiers to given map territory as orders
		} else if (splitCommand[0] == "ATTACK") {
			// attack given map territory as orders
		} else if (splitCommand[0] == "USECARD") {
			// use a card from player's hand as orders
		} else if (splitCommand[0] == "DRAWCARD") {
			// draw a card from the deck to player's hand
		} else if (splitCommand[0] == "VIEWHAND") {
			// view player's hand
		} else if (splitCommand[0] == "VIEWORDERS") {
			// view player's current orders
		} else if (splitCommand[0] == "MOVEORDERS") {
			// move an order to a different index in the player's order list
		} else if (splitCommand[0] == "REMOVEORDER") {
			// remove an order from the player's order list
		} else if (splitCommand[0] == "EXECUTEORDERS") {
			// execute all orders in the player's order list
		} else if (splitCommand[0] == "QUIT") {
			// quit the game
		} else if (splitCommand[0] == "SHOWMAP") {
			// show the current map
		} else if (splitCommand[0] == "SHOWPLAYERS") {
			// show all players in the game
		} else {
			std::cout << "Unknown command: " << command << std::endl;
		}
	}
}

void GameEngine::setupGame() {
	// Initial game setup logic
	std::cout << "Setting up the game..." << std::endl;
	std::cout << "Please enter player name: ";
	std::string playerName;
	std::getline(std::cin, playerName);
	addPlayer(playerName);
	viewPlayers();
}

void GameEngine::runGame() {
	// Main game loop
	while (!isGameOver()) {
		std::string command;
		std::cout
			<< "Enter command or type 'MANUAL' to view possible commands: ";
		std::getline(std::cin, command);
		GameEngine::command(command);
	}
}

void GameEngine::changeState(State* newState) {
	if (currentState) {
		delete currentState; // Clean up the old state
	}
	currentState = newState;
}

void GameEngine::addPlayer(const std::string &playerName) {
  std::cout << "Inside add players: " << std::endl;
	Player* newPlayer = new Player(playerName);
	players.push_back(*newPlayer);
  delete newPlayer;
	std::cout << "Added player: " << playerName << std::endl;
}

void GameEngine::removePlayer(const std::string &playerName) {
	for (int i = 0; i < players.size(); ++i) {
		if (players[i].getName() == playerName) {
			// Shift players down to fill the gap
			for (int j = i; j < players.size() - 1; ++j) {
				players[j] = players[j + 1];
			}
			std::cout << "Removed player: " << playerName << std::endl;
			return;
		}
	}
	std::cout << "Player not found: " << playerName << std::endl;
}

void GameEngine::viewPlayers() const {
	std::cout << "Current players in the game:" << std::endl;
	for (int i = 0; i < players.size(); ++i) {
    std::cout << "- " << players[i].getName() << std::endl;
  }
}

bool GameEngine::isGameOver() const {
	return getCurrentStateName() == "Win" || getCurrentStateName() == "End";
}

std::string GameEngine::getCurrentStateName() const {
	return currentState ? currentState->getName() : "Unknown State";
}