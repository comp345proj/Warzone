#include "GameEngine.h"
#include <algorithm>

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
			if (currentState->getSubState() == "Reinforcement") {
				if (splitCommand[0] == "MANUAL") {
					std::cout << "------- REINFORCEMENT MANUAL "
								 "-------\nPossible commands:\n"
							  << "DEPLOY <# of armies> <owned territory>\n"
							  << "ENDDEPLOY\n"
							  << "QUIT\n";
				} else if (splitCommand[0] == "DEPLOY") {
					// deploy soldiers to given map territory of player
					int deployAmount = std::stoi(splitCommand[1]);
					std::string territoryName = splitCommand[2];
					// Check if currentPlayer owns the territory
					bool ownsTerritory = false;
					for (Territory* territory :
						 currentPlayer->getTerritories()) {
						if (territory->getName() == territoryName) {
							ownsTerritory = true;
							int currentArmies = territory->getNbOfArmies();
							territory->setNbOfArmies(currentArmies +
													 deployAmount);
							std::cout << "Deployed " << deployAmount
									  << " armies to " << territoryName
									  << std::endl;
							break;
						}
					}
				} else if (splitCommand[0] == "QUIT") {
					std::cout << "Quitting the game." << std::endl;
					currentState->setState("END");
				} else {
					std::cout << "Unknown command: " << command << std::endl;
				}
			} else if (currentState->getSubState() == "IssueOrders") {
				if (splitCommand[0] == "MANUAL") {
					std::cout
						<< "------- ISSUE ORDERS MANUAL -------\nPossible "
						   "commands:\n"
						<< "DEPLOY <# of armies> <source territory> <target "
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
				} else if (splitCommand[0] == "USECARD") {
					// use a card from player's hand as orders
				} else if (splitCommand[0] == "DRAWCARD") {
					// draw a card from the deck to player's hand
				} else if (splitCommand[0] == "VIEWHAND") {
					// view player's hand
					std::cout << "\n"
							  << currentPlayer->getName() << "'s Hand:\n";
					for (int i = 0;
						 i < currentPlayer->getHand()->getCards().size(); i++) {
						std::cout << i + 1 << ". "
								  << *(currentPlayer->getHand()->getCards()[i])
								  << std::endl;
					}
				} else if (splitCommand[0] == "VIEWORDERS") {
					// view player's current orders
					std::cout << "\n"
							  << currentPlayer->getName()
							  << "'s Orders List:\n";
					for (int i = 0; i < currentPlayer->getOrders().size();
						 i++) {
						std::cout << i + 1 << ". "
								  << *(currentPlayer->getOrders()[i])
								  << std::endl;
					}
				} else if (splitCommand[0] == "VIEWMAP") {
					// view the current map
				} else if (splitCommand[0] == "VIEWPLAYERS") {
					viewPlayers(); // view all players in the game
				} else if (splitCommand[0] == "MOVEORDERS") {
					// move an order to a different index in the player's order
				} else if (splitCommand[0] == "QUIT") {
					// quit the game
					std::cout << "Quitting the game." << std::endl;
					currentState->setState("END");
				} else {
					std::cout << "Unknown command: " << command << std::endl;
				}
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
		std::shuffle(allTerritories.begin(), allTerritories.end(), gen);

		int numPlayers = players.size();
		size_t assignments =
			std::min((size_t)numPlayers, allTerritories.size());
		for (size_t i = 0; i < assignments; ++i) {
			Territory* territory = allTerritories[i];
			Player &currentPlayerRef = players[i];
			currentPlayerRef.addTerritory(territory);
			std::cout << "Assigned territory " << territory->getName()
					  << " to player " << currentPlayerRef.getName()
					  << std::endl;
		}
		std::cout << "Territory assignment complete." << std::endl;
	}
}

void GameEngine::assignReinforcement() {
	currentState->setSubState("Reinforcement");
	bool assigningReinforcements = true;
	int deployableArmies = 5;
	while (assigningReinforcements) {
		std::cout << "\nReinforce your territories with 5 armies" << std::endl;
		std::cout << "Your territories are: " << std::endl;
		for (Territory* territory : currentPlayer->getTerritories()) {
			std::cout << "- " << territory->getName() << " ("
					  << territory->getNbOfArmies() << ")" << std::endl;
		}
		std::cout << "Deployable armies left: " << deployableArmies
				  << std::endl;
		std::cout << "(DEPLOY <# of armies> <territory>)" << std::endl;
		std::cout << "Enter command ENDDEPLOY to finish deployment"
				  << std::endl;

		std::string command;
		std::getline(std::cin, command);
		std::vector<std::string> splitCommand = splitString(command, ' ');
		if (splitCommand[0] == "MANUAL") {
			this->command("MANUAL");
		} else if (splitCommand[0] == "DEPLOY") {
			int deployAmount = std::stoi(splitCommand[1]);
			if (deployAmount <= deployableArmies && deployAmount > 0) {
				this->command(command);
				deployableArmies -= deployAmount;
			} else {
				std::cout << "Invalid deploy amount. You have "
						  << deployableArmies << " armies left to deploy."
						  << std::endl;
			}
		} else if (splitCommand[0] == "ENDDEPLOY") {
			assigningReinforcements = false;
			currentState->setSubState("IssueOrders");
		} else {
			std::cout << "Unknown command: " << command << std::endl;
		}
	}
}

void GameEngine::issueOrders() {
	currentState->setSubState("IssueOrders");
	bool issuingOrders = true;
	while (issuingOrders) {
		std::cout << "\nYou can now issue orders." << std::endl;
		std::cout << "Type MANUAL for list of commands." << std::endl;
		std::string command;
		std::getline(std::cin, command);
		std::vector<std::string> splitCommand = splitString(command, ' ');
		if (splitCommand[0] == "EXECUTEORDERS") {
			issuingOrders = false;
		} else {
			this->command(command);
		}
	}
}

void GameEngine::executeOrders() {
	currentState->setSubState("ExecuteOrders");
	std::cout << "\nExecuting orders for " << currentPlayer->getName() << "..."
			  << std::endl;
	// Execute all orders for the current player
	currentPlayer->getOrders();
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
			assignReinforcement();
			issueOrders();
			executeOrders();
			// Check for win condition after each turn
		}
	}
}

bool GameEngine::isGameOver() const {
	return currentState->getState() == "Win" ||
		   currentState->getState() == "End";
}