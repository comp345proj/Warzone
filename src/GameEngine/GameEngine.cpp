#include "GameEngine.h"

//---------------------------State-------------------------------

State::State(const std::string &state)
	: state(state), subState(""), currentPlayerTurn("") {}
State::State(const std::string &state, const std::string &subState)
	: state(state), subState(""), currentPlayerTurn("") {}
State::State(const State &other)
	: state(other.state), currentPlayerTurn(other.currentPlayerTurn) {}

State &State::operator=(const State &other) {
	if (this != &other) {
		state = other.state;
		currentPlayerTurn = other.currentPlayerTurn;
	}
	return *this;
}

State::~State() = default;

std::string State::getState() const {
	return state;
}

void State::setState(const std::string &newState) {
	state = newState;
}

std::string State::getSubState() const {
	return subState;
}

void State::setSubState(const std::string &newSubState) {
	subState = newSubState;
}

std::string State::getCurrentPlayerTurn() const {
	return currentPlayerTurn;
}

void State::setCurrentPlayerTurn(const std::string &playerName) {
	currentPlayerTurn = playerName;
}

//---------------------------GameEngine-------------------------------

GameEngine::GameEngine()
	: currentState(new State("STARTUP")), currentMap(nullptr),
	  mapLoader(nullptr), currentMapPath(""), currentPlayer(nullptr) {}

GameEngine::GameEngine(const GameEngine &other)
	: currentState(new State(*other.currentState)), players(other.players),
	  currentPlayer(other.currentPlayer),
	  currentMap(new Map(*other.currentMap)),
	  mapLoader(new MapLoader(*other.mapLoader)) {}

GameEngine &GameEngine::operator=(const GameEngine &other) {
	if (this != &other) {
		delete currentState;
		delete currentMap;
		currentState = new State(*other.currentState);
		players = other.players;
		currentPlayer = other.currentPlayer;
		currentMap = new Map(*other.currentMap);
		mapLoader = new MapLoader(*other.mapLoader);
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
		// ---------- STARTUP STATE COMMANDS ----------
		if (currentState->getState() == "STARTUP") {
			if (splitCommand[0] == "MANUAL") {
				std::cout
					<< "\n------- STARTUP MANUAL -------\nPossible commands:\n"
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
				currentMapPath = splitCommand[1] == "DEFAULT"
									 ? "res/World.map"
									 : "res/" + splitCommand[1];

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
				delete mapLoader;
				mapLoader = new MapLoader(mapPath.string());
				currentMap = mapLoader->loadMap();
			} else if (splitCommand[0] == "ADDPLAYER") {
				addPlayer(splitCommand[1]); // add player function call
			} else if (splitCommand[0] == "START") {
				if (currentMap && players.size() >= 2) {
					std::cout << "Starting the game..." << std::endl;
					currentState->setState("PLAY");
				} else {
					std::cout
						<< "\nCannot start game. Ensure map is loaded and "
						   "at least two players are added."
						<< std::endl;
				}
			} else if (splitCommand[0] == "QUIT") {
				std::cout << "Quitting the game startup." << std::endl;
				currentState->setState("END");
			} else {
				std::cout << "Unknown command in startup state: " << command
						  << std::endl;
			}
			// ---------- ASSIGNREINFORCEMENTS STATE COMMANDS ----------
		} else if (currentState->getState() == "PLAY") {
			if (splitCommand[0] == "MANUAL") {
				std::cout
					<< "------- PLAY MANUAL -------\nPossible commands:\n"
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
					<< "VIEWMAP\n"
					<< "VIEWPLAYERS\n"
					<< "VIEWORDERS\n"
					<< "MOVEORDERS\n"
					<< "REMOVEORDER <order index>\n"
					<< "EXECUTEORDERS\n"
					<< "QUIT\n";
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
}

void GameEngine::startupGame() {
	currentState->setSubState("MapLoaded");
	std::cout << "Setting up the game..." << std::endl;
	std::cout << "Please enter map filename (or type DEFAULT for World.map): ";
	std::string mapFilename;
	std::getline(std::cin, mapFilename);
	this->command("LOADMAP " + mapFilename);
	currentMapPath =
		mapFilename == "DEFAULT" ? "res/World.map" : "res/" + mapFilename;
	std::cout << "Now you may add players or change the map." << std::endl;
	currentState->setSubState("PlayersAdded");
	while (currentState->getState() == "STARTUP") {
		std::cout
			<< "\nSTARTUP GAME REQUIREMENTS:\n - Map filename\n - Players "
			   "(minimum 2)\n - type START to begin\n - type MANUAL for "
			   "startup commands.\n";
		std::string command;
		std::getline(std::cin, command);
		this->command(command);

		viewstartupDetails();
	}
	assignTerritories();
}

void GameEngine::viewstartupDetails() const {
	std::cout << "\nCurrent Game Details:\n";
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

void GameEngine::assignTerritories() {
	if (currentMap && players.size() >= 2) {
		std::cout << "\nAssigning territories to players..." << std::endl;
		// assign random territory to players
		std::vector<Territory*> allTerritories;
		for (Continent* continent : currentMap->getContinents()) {
			for (Territory* territory : continent->getTerritories()) {
				allTerritories.push_back(territory);
			}
		}

		if (allTerritories.empty()) {
			std::cout << "No territories available on the map to assign."
					  << std::endl;
			return;
		}

		// randomly assign territories to players
		std::random_device rd;
		std::mt19937 gen(rd());

		for (Player player : players) {
			if (allTerritories.empty()) {
				break; // No more territories to assign
			}
			std::uniform_int_distribution<> distr(0, allTerritories.size() - 1);
			int randomIndex = distr(gen);
			Territory* selectedTerritory = allTerritories[randomIndex];
			player.addTerritory(selectedTerritory);
			std::cout << "Assigned territory " << selectedTerritory->getName()
					  << " to player " << player.getName() << std::endl;
			allTerritories.erase(allTerritories.begin() + randomIndex);
		}
	}
}

void GameEngine::assignReinforcement(){
  // assign reinforcements to current player based on owned territories and assign number
}

void GameEngine::viewPlayers() const {
	std::cout << "Current players in the game:" << std::endl;
	for (int i = 0; i < players.size(); ++i) {
		std::cout << "- " << players[i].getName() << std::endl;
	}
}

void GameEngine::runGame() {
	startupGame();
	// Main game loop
	while (!isGameOver()) {
		// Player turn rotation
		for (int i = 0; i < players.size(); ++i) {
			currentPlayer = &players[i];
			currentState->setCurrentPlayerTurn(currentPlayer->getName());
			std::cout << "\nIt's " << currentPlayer->getName() << "'s turn."
					  << std::endl;
			bool playerTurn = true;
			// Player turn loop
			while (playerTurn) {
				assignReinforcement();
				// issueOrders();
				// executeOrders();
				//  Check for win condition after each turn
			}
		}
	}
}

bool GameEngine::isGameOver() const {
	return currentState->getState() == "Win" ||
		   currentState->getState() == "End";
}