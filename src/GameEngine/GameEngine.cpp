#include "GameEngine.h"
#include <algorithm>
using std::cin;

std::string commandTypeToString(CommandType state)
{
	switch (state) {
	case CommandType::LOAD_MAP:
		return "LOADMAP";
	case CommandType::VALIDATE_MAP:
		return "VALIDATEMAP";
	case CommandType::ADD_PLAYER:
		return "ADDPLAYER";
	case CommandType::ASSIGN_COUNTRIES:
		return "ASSIGNCOUNTRIES";
	case CommandType::ISSUE_ORDER:
		return "ISSUEORDER";
	case CommandType::EXECUTE_ORDER:
		return "EXECUTEORDER";
	case CommandType::END_ISSUE_ORDERS:
		return "ENDISSUEORDERS";
	case CommandType::END_EXECUTE_ORDERS:
		return "ENDEXECUTEORDERS";
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
		return "Start";
	case StateType::MAP_LOADED:
		return "MapLoaded";
	case StateType::MAP_VALIDATED:
		return "MapValidated";
	case StateType::PLAYERS_ADDED:
		return "PlayersAdded";
	case StateType::ASSIGN_REINFORCEMENT:
		return "AssignReinforcement";
	case StateType::ISSUE_ORDERS:
		return "IssueOrders";
	case StateType::EXECUTE_ORDERS:
		return "ExecuteOrders";
	case StateType::WIN:
		return "Win";
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
	return state->getState() == "END";
}