#include "GameEngine.h"

GameEngine::GameEngine()
	: currentState(new State("Start")),
	  currentMap(new MapLoader("../../res/World.map")) {}

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

void GameEngine::changeState(State* newState) {
	if (currentState) {
		delete currentState; // Clean up the old state
	}
	currentState = newState;
	std::cout << "Changed state to: " << getCurrentStateName() << std::endl;
}

void GameEngine::addPlayer(const std::string &playerName) {
	Player* newPlayer = new Player(playerName);
	players[numPlayers++] = *newPlayer;
	std::cout << "Added player: " << playerName << std::endl;
}

bool GameEngine::isGameOver() const {
	return getCurrentStateName() == "Win" || getCurrentStateName() == "End";
}

std::string GameEngine::getCurrentStateName() const {
	return currentState ? currentState->getName() : "Unknown State";
}