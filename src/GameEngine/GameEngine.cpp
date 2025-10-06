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
	: currentState(new State("SETUP")), currentMap(nullptr),
	  currentPlayer(nullptr) {}

GameEngine::GameEngine(const GameEngine &other)
	: currentState(new State(*other.currentState)), players(other.players),
	  currentPlayer(other.currentPlayer),
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
		// ---------- SETUP STATE COMMANDS ----------
		if (currentState->getName() == "SETUP") {
			if (splitCommand[0] == "MANUAL") {
				std::cout
					<< "\n------- SETUP MANUAL -------\nPossible commands:\n"
					<< "LOADMAP <filename> (.map file/DEFAULT for World.map)\n"
					<< "ADDPLAYER <playername>\n"
					<< "START\n"
					<< "QUIT\n"
					<< std::endl;
			} else if (splitCommand[0] == "LOADMAP") {
				namespace fs = std::filesystem;
				fs::path currentPath = fs::current_path();
				fs::path mapPath;

				if (splitCommand.size() < 2) {
					std::cout
						<< "Error: No filename provided for LOADMAP command."
						<< std::endl;
					return;
				}

				if (splitCommand[1] == "DEFAULT") {
					mapPath = currentPath / "res" / "World.map";
				} else {
					mapPath = currentPath / "res" / splitCommand[1];
				}

				if (!fs::exists(mapPath)) {
					std::cout << "Error: Map file does not exist at " << mapPath
							  << std::endl;
					return;
				}

				delete currentMap;
				currentMap = new MapLoader(mapPath.string());
				currentMap->loadMap();
			} else if (splitCommand[0] == "ADDPLAYER") {
				addPlayer(splitCommand[1]); // add player function call
			} else if (splitCommand[0] == "START") {
				if (currentMap && players.size() >= 2) {
					std::cout << "Starting the game..." << std::endl;
					changeState(new State("ASSIGNREINFORCEMENTS"));
				} else {
					std::cout
						<< "\nCannot start game. Ensure map is loaded and "
						   "at least two players are added."
						<< std::endl;
				}
			} else if (splitCommand[0] == "QUIT") {
				std::cout << "Quitting the game setup." << std::endl;
				changeState(new State("END"));
			} else {
				std::cout << "Unknown command in SETUP state: " << command
						  << std::endl;
			}
			// ---------- ASSIGNREINFORCEMENTS STATE COMMANDS ----------
		} else if (splitCommand[0] == "MANUAL") {
			std::cout << "Possible commands:\n"
					  << "startup  commands:\n"
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
	std::cout << "Setting up the game..." << std::endl;
	while (currentState->getName() == "SETUP") {

		std::cout << "\nSETUP GAME REQUIREMENTS:\n - Map filename\n - Players "
					 "(minimum 2)\n - type START to begin\n - type MANUAL for "
					 "SETUP commands.\n";
		std::string command;
		std::getline(std::cin, command);
		this->command(command);

		viewSetupDetails();
	}
}

void GameEngine::viewSetupDetails() const {
	std::cout << "Current Game Details:\n";
	if (currentMap) {
		std::cout << "- Map: " << currentMapPath << std::endl;
	} else {
		std::cout << "- Map: Not loaded" << std::endl;
	}
	std::cout << "- Players (" << players.size() << "): ";
	for (const auto &player : players) {
		std::cout << player.getName() << ", ";
	}
	std::cout << std::endl;
}

void GameEngine::runGame() {
	// Main game loop
	while (!isGameOver()) {
		std::string command;
		std::cout << "Enter command or type 'MANUAL' to view commands:\n";
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