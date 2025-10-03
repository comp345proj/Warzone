#include "GameEngine.h"
#include "Map/MapDriver.h"
#include "Player/Player.h"

GameEngine::GameEngine() : currentState(new State("Start")) {}

void GameEngine::command(const std::string &command) {
	if (currentState) {
		if (command == "MANUAL") {
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
		} else if (command == "LOADMAP") {

		} else if (command == "ADDPLAYER") {
		} else if (command == "STARTDEPLOYMENT") {
		} else if (command == "DEPLOY") {
		} else if (command == "ATTACK") {
		} else if (command == "USECARD") {
		} else if (command == "DRAWCARD") {
		} else if (command == "VIEWHAND") {
		} else if (command == "VIEWORDERS") {
		} else if (command == "MOVEORDERS") {
		} else if (command == "REMOVEORDER") {
		} else if (command == "EXECUTEORDERS") {
		} else if (command == "QUIT") {
		} else if (command == "SHOWMAP") {
		} else if (command == "SHOWPLAYERS") {
		} else {
		}
	}
}

void GameEngine::changeState(State* newState) {
	if (currentState) {
		delete currentState; // Clean up the old state
	}
	// TODO add state transitions depending on user choice
	currentState = newState;
	std::cout << "Changed state to: " << getCurrentStateName() << std::endl;
}

bool GameEngine::isGameOver() const {
	return getCurrentStateName() == "Win" || getCurrentStateName() == "End";
}

std::string GameEngine::getCurrentStateName() const {
	return currentState ? currentState->getName() : "Unknown State";
}
