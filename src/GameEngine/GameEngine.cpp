#include "GameEngine.h"
#include <algorithm>
using std::cin;

std::string gameStateTypeToString(CommandType state) {
    switch (state) {
        case CommandType::START:
            return "START";
        case CommandType::MAP_LOADED:
            return "MAP_LOADED";
        case CommandType::MAP_VALIDATED:
            return "MAP_VALIDATED";
        case CommandType::PLAYERS_ADDED:
            return "PLAYERS_ADDED";
        case CommandType::ASSIGN_REINFORCEMENT:
            return "ASSIGN_REINFORCEMENT";
        case CommandType::ISSUE_ORDERS:
            return "ISSUE_ORDERS";
        case CommandType::EXECUTE_ORDERS:
            return "EXECUTE_ORDERS";
        case CommandType::WIN:
            return "WIN";
        case CommandType::END:
            return "END";
        default:
            return "UNKNOWN";
    }
}

void printInvalidCommandError()
{
    std::cout << "\nInvalid Command entered..." << std::endl;
};

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
	: state(new State("Start")), currentMap(nullptr), mapLoader(nullptr),
	  currentMapPath(new std::string()), currentPlayer(nullptr) {}

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
	} else if (splitCommand[0] == "MANUAL" || splitCommand[0] == "manual") {
		std::cout << "\n------- MANUAL -------\n"
				  << "LOADMAP <filename> (.map file/DEFAULT for World.map)\n"
				  << "VALIDATEMAP\n"
				  << "ADDPLAYER <playername>\n"
				  << "START\n"
				  << "REPLAY\n"
				  << "QUIT" << std::endl;
		// Load Map Command
	} else if (splitCommand[0] == "LOADMAP" || splitCommand[0] == "loadmap") {
		if (state->getState() == "Start" || state->getState() == "MapLoaded") {
			// read .map from res/ directory
			namespace fs = std::filesystem;
			fs::path currentPath = fs::current_path();
			fs::path mapPath;

			*currentMapPath = 
				(splitCommand[1] != "DEFAULT" || splitCommand[1] != "default")
					? "res/" + splitCommand[1]
					: "res/World.map";

			if (splitCommand[1] == "DEFAULT" || splitCommand[1] == "default") {
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
	} else if (splitCommand[0] == "VALIDATEMAP" ||
			   splitCommand[0] == "validatemap") {
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
	} else if (splitCommand[0] == "ADDPLAYER" ||
			   splitCommand[0] == "addplayer") {
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
	} else if (splitCommand[0] == "GAMESTART" ||
			   splitCommand[0] == "gamestart") {
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
	} else if (splitCommand[0] == "REPLAY" || splitCommand[0] == "replay") {
		if (state->getState() == "Win") {
			std::cout << "Replaying the game from the beginning..."
					  << std::endl;
			state->setState("Start");
		} else {
			std::cout << "Error: REPLAY command is only valid in Win state."
					  << std::endl;
		}
		// Quit Command
	} else if (splitCommand[0] == "QUIT" || splitCommand[0] == "quit") {
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
	players.push_back(newPlayer);
	delete newPlayer;
	std::cout << "Added player: " << playerName << std::endl;
}

/// @brief Function which checks if the game is over based on state
/// @return
bool GameEngine::isGameOver() const {
	return state->getState() == "End";
}