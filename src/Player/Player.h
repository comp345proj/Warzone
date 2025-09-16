#pragma once
#include <memory>
#include <string>
#include <vector>

// Forward declarations
class Territory;
class Card;
class Order;

class Player {
  public:
	Player(const std::string &name);
	Player(const Player &other);			// Copy constructor
	Player &operator=(const Player &other); // Assignment operator
	~Player();

	std::vector<Territory*> toDefend();
	std::vector<Territory*> toAttack();
	void issueOrder();

	friend std::ostream &operator<<(std::ostream &os, const Player &player);

  private:
	std::string name;
	std::vector<Territory*> territories;
	std::vector<Card*> hand;
	std::vector<Order*> ordersList;
};
