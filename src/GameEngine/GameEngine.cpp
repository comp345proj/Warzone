#include "GameEngine.h"
#include <algorithm>

//---------------------------State-------------------------------

// Constructors, destructor, copy constructor, assignment operator
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

// Getters and setters
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

// Constructors, destructor, copy constructor, assignment operator
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

// Getters and setters
std::string GameEngine::getState() {
	return state->getState();
}
void GameEngine::setState(const std::string &newState) {
	state->setState(newState);
}

/// @brief Primitive command function to handle user input
/// @param command
void GameEngine::command(const std::string &command) {
	std::vector<std::string> splitCommand = splitString(command, ' ');
	// Helper State Command
	if (splitCommand[0] == "STATE") {
		std::cout << "Current State: " << state->getState();
		if (!state->getCurrentPlayerTurn().empty()) {
			std::cout << " | Current Player: " << state->getCurrentPlayerTurn();
		}
		std::cout << std::endl;
		return;
		// Helper Manual Command
	} else if (splitCommand[0] == "MANUAL") {
		std::cout << "\n------- MANUAL -------\n"
				  << "LOADMAP <filename> (.map file/DEFAULT for World.map)\n"
				  << "VALIDATEMAP\n"
				  << "ADDPLAYER <playername>\n"
				  << "START\n"
				  << "REPLAY\n"
				  << "QUIT" << std::endl;
		// Load Map Command
	} else if (splitCommand[0] == "LOADMAP") {
		if (state->getState() == "Start" || state->getState() == "MapLoaded") {
			// read .map from res/ directory
			namespace fs = std::filesystem;
			fs::path currentPath = fs::current_path();
			fs::path mapPath;

			currentMapPath = splitCommand[1] != "DEFAULT"
								 ? "res/" + splitCommand[1]
								 : "res/World.map";

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
		// Validate Map Command
	} else if (splitCommand[0] == "VALIDATEMAP") {
		if (state->getState() == "MapLoaded") {
			if (currentMap) {
				if (currentMap->validate()) {
					std::cout << "Map is valid." << std::endl;
					state->setState("MapValidated");
				} else {
					std::cout << "Map is invalid." << std::endl;
				}
			} else {
				std::cout << "Error: No map loaded to validate." << std::endl;
			}
		} else {
			std::cout << "Error: VALIDATEMAP command is only valid in "
						 "MapLoaded state."
					  << std::endl;
			return;
		}
		// Add Player Command
	} else if (splitCommand[0] == "ADDPLAYER") {
		if (state->getState() == "MapValidated" ||
			state->getState() == "PlayersAdded") {
			addPlayer(splitCommand[1]);
			state->setState("PlayersAdded");
		} else {
			std::cout << "Error: ADDPLAYER command is only valid in "
						 "MapValidated or PlayersAdded state."
					  << std::endl;
			return;
		}
		// Start Game Command
	} else if (splitCommand[0] == "GAMESTART") {
		if (state->getState() == "PlayersAdded") {
			if (currentMap && players.size() >= 2) {
				std::cout << "Starting the game..." << std::endl;
				state->setState("AssignReinforcement");
			} else {
				std::cout << "\nCannot start game. Ensure map is loaded and "
							 "at least two players are added."
						  << std::endl;
			}
		}
		// Replay Command
	} else if (splitCommand[0] == "REPLAY") {
		if (state->getState() == "Win") {
			std::cout << "Replaying the game from the beginning..."
					  << std::endl;
			state->setState("Start");
		} else {
			std::cout << "Error: REPLAY command is only valid in Win state."
					  << std::endl;
		}
		// Quit Command
	} else if (splitCommand[0] == "QUIT") {
		if (state->getState() == "Win") {
			std::cout << "Quitting the game startup." << std::endl;
			state->setState("End");
		} else {
			std::cout << "Error: QUIT command is only valid in Win state."
					  << std::endl;
		}
	} else {
		std::cout << "Error: Unknown command." << std::endl;
		return;
	}
}

/// @brief Function which pushes a new player to the players vector
/// @param playerName
void GameEngine::addPlayer(const std::string &playerName) {
	Player* newPlayer = new Player(playerName);
	players.push_back(*newPlayer);
	delete newPlayer;
	std::cout << "Added player: " << playerName << std::endl;
}

/* Unnecessary for A1 */
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

/* Unnecessary for A1 */
void GameEngine::assignReinforcement() {}

/* Unnecessary for A1 */
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

/* Unnecessary for A1 */
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

/// @brief Function which checks if the game is over based on state
/// @return
bool GameEngine::isGameOver() const {
	return state->getState() == "End";
}