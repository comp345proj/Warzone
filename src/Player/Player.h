#pragma once
#include "Cards/Cards.h"
#include "Map/Map.h"
#include "Orders/Orders.h"
#include <string>
#include <vector>

// Forward declaration
class Territory;
class Card;
class Hand;
class Deck;
class OrdersList;

class Player {
  private:
	std::string* name;
	std::vector<Territory*> territories;
	std::unique_ptr<Hand> hand;
	OrdersList* ordersList = nullptr;
	int* reinforcementPool;

  public:
	Player(const std::string &name, Hand* hand = nullptr);
	Player(const Player &other);
	Player &operator=(const Player &other);
	~Player();

	// Territory management
	void addTerritory(Territory* territory);
	void removeTerritory(Territory* territory);
	const std::vector<Territory*> &getTerritories() const;

	// Card management
	void addCard(Card* card);
	void removeCard(Card* card);
	const std::vector<Card*> &getCards() const;

	Hand* getHand() const;

	void setHand(Hand* newHand);

	// Order management
	void addOrder(Order* order);

	// Strategy methods
	std::vector<Territory*>
	toDefend(); // Returns territories to be defended in priority
	std::vector<Territory*>
	toAttack(); // Returns neighboring territories to be attacked in priority

	// Order issuing
	void issueOrder(Deck* deck, Card* playedCard = nullptr);
	void issueAdvanceOrder(Territory* from, Territory* to, int numArmies);

	// Getters
	const std::string &getName() const;
	OrdersList* getOrdersList() const;
	int getReinforcementPool() const;

	// Setters
	void setReinforcementPool(int amount);

	friend std::ostream &operator<<(std::ostream &os, const Player &player);
};
