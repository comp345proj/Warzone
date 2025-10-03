#pragma once
#include <iostream>
#include <string>

class State {
  public:
	State(const std::string &name);
	State(const State &other);			  // Copy constructor
	State &operator=(const State &other); // Assignment operator
	~State();

	std::string getName() const;

  private:
	std::string name;
	/* State Name possibilites:
   * Start
	 * MapLoaded
	 * MapValidated
	 * PlayersAdded
	 * AssignReinforcement
	 * IssueOrders
	 * ExecuteOrders
	 * Win
	 * End
	 */
};

class GameEngine {
  public:
	GameEngine();
	GameEngine(const GameEngine &other);			// Copy constructor
	GameEngine &operator=(const GameEngine &other); // Assignment operator
	~GameEngine();

	void command(const std::string &command);
  /*
    * Possible commands:
    * MANUAL - Display this manual
    * startup:
    * LOADMAP <filename>
    * ADDPLAYER <playername>
    * 
    * play:
    * STARTDEPLOYMENT <# of armies> <target territory>
    * DEPLOY <# of armies> <source territory> <target territory>
    * ATTACK <# of armies> <source territory> <target territory>
    * USECARD <card type> <location>
      * ex// USECARD BOMB <territory name>
      * ex// USECARD REINFORCEMENT <territory name>
      * ex// USECARD BLOCKADE <territory name>
      * ex// USECARD AIRLIFT <source territory> <target territory>
      * ex// USECARD DIPLOMACY <player name>
    * DRAWCARD
    * VIEWHAND
    * VIEWORDERS
    * MOVEORDERS
    * REMOVEORDER <order index>
    * EXECUTEORDERS
    * QUIT
    * SHOWMAP
    * SHOWPLAYERS
  */
 
	void changeState(State* newState);

	friend std::ostream &operator<<(std::ostream &output,
									const GameEngine &gameEngine);

	std::string getCurrentStateName() const;

	bool isGameOver() const;

  private:
	State* currentState;
};
