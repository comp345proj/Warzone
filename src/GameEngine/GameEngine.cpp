#include "GameEngine.h"
#include <algorithm>

//---------------------------State-------------------------------

State::State(const std::string &state) : state(state), currentPlayerTurn("") {}
State::State(const std::string &state, const std::string &subState)
	: state(state), currentPlayerTurn("") {}
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

std::string State::getCurrentPlayerTurn() const {
	return currentPlayerTurn;
}

void State::setCurrentPlayerTurn(const std::string &playerName) {
	currentPlayerTurn = playerName;
}

//---------------------------GameEngine-------------------------------

GameEngine::GameEngine()
	: state(new State("Start")), currentMap(nullptr), mapLoader(nullptr),
	  currentMapPath(""), currentPlayer(nullptr) {}

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
	players.clear();
	currentPlayer = nullptr;
}

std::string GameEngine::getState() {
	return state->getState();
}
void GameEngine::setState(const std::string &newState) {
	state->setState(newState);
}

void GameEngine::command(const std::string &command) {
	// split the command by space character and then store into an array
	std::vector<std::string> splitCommand = splitString(command, ' ');
	if (splitCommand[0] == "STATE") {
		std::cout << "Current State: " << state->getState();
		if (!state->getCurrentPlayerTurn().empty()) {
			std::cout << " | Current Player: "
					  << state->getCurrentPlayerTurn();
		}
		std::cout << std::endl;
		return;
	} else if (splitCommand[0] == "MANUAL") {
		std::cout << "\n------- MANUAL -------\n"
				  << "LOADMAP <filename> (.map file/DEFAULT for World.map)\n"
				  << "VALIDATEMAP\n"
				  << "ADDPLAYER <playername>\n"
				  << "START\n"
				  << "REPLAY\n"
				  << "QUIT\n"
				  << std::endl;
	} else if (splitCommand[0] == "LOADMAP") {
		if (state->getState() == "Start" ||
			state->getState() == "MapLoaded") {
			namespace fs = std::filesystem;
			fs::path currentPath = fs::current_path();
			fs::path mapPath;

			if (splitCommand.size() < 2) {
				std::cout << "Error: No filename provided for LOADMAP command."
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
			state->setState("MapLoaded");
		} else {
			std::cout << "Error: LOADMAP command is only valid in Start or "
						 "MapLoaded state."
					  << std::endl;
			return;
		}
	} else if (splitCommand[0] == "ADDPLAYER") {
		addPlayer(splitCommand[1]);
	} else if (splitCommand[0] == "START") {
		if (currentMap && players.size() >= 2) {
			std::cout << "Starting the game..." << std::endl;
			state->setState("PLAY");
		} else {
			std::cout << "\nCannot start game. Ensure map is loaded and "
						 "at least two players are added."
					  << std::endl;
		}
	} else if (splitCommand[0] == "REPLAY") {
		std::cout << "Replaying the game from the beginning..." << std::endl;
	} else if (splitCommand[0] == "QUIT") {
		std::cout << "Quitting the game startup." << std::endl;
		state->setState("END");
	} else {
		std::cout << "Unknown command in startup state: " << command
				  << std::endl;
	}
}

void GameEngine::startupGame() {
	std::cout << "Setting up the game..." << std::endl;
	std::cout << "Please enter map filename (or type DEFAULT for World.map): ";
	std::string mapFilename;
	std::getline(std::cin, mapFilename);
	this->command("LOADMAP " + mapFilename);
	currentMapPath =
		mapFilename == "DEFAULT" ? "res/World.map" : "res/" + mapFilename;
	std::cout << "Now you may add players or change the map." << std::endl;
	while (state->getState() == "Start") {
		std::cout
			<< "\nSTARTUP GAME REQUIREMENTS:\n - Map filename\n - Players "
			   "(minimum 2)\n - type START to begin\n - type MANUAL for "
			   "startup commands.\n";
		std::string command;
		std::getline(std::cin, command);
		this->command(command);
	}
	assignTerritories();
}

void GameEngine::addPlayer(const std::string &playerName) {
	Player* newPlayer = new Player(playerName);
	players.push_back(*newPlayer);
	delete newPlayer;
	std::cout << "Added player: " << playerName << std::endl;
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

void GameEngine::assignReinforcement() {}

void GameEngine::runGame() {
	startupGame();
	// Main game loop
	while (!isGameOver()) {
		// Player turn rotation
		for (int i = 0; i < players.size(); ++i) {
			currentPlayer = &players[i];
			state->setCurrentPlayerTurn(currentPlayer->getName());
			std::cout << "\nIt's " << currentPlayer->getName() << "'s turn."
					  << std::endl;
			assignReinforcement();
			// Check for win condition after each turn
		}
	}
}

bool GameEngine::isGameOver() const {
	return state->getState() == "Win" ||
		   state->getState() == "End";
}