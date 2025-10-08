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
	std::vector<Territory*> territories; // Owned territories
	std::unique_ptr<Hand> hand;			 // Player's hand of cards
	OrdersList* ordersList = nullptr;		 // List of orders
	
  public:
	Player(const std::string &name, Hand* hand = nullptr);
	Player(const Player &other);			// Copy constructor
	Player &operator=(const Player &other); // Assignment operator
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

	// Set the player's hand (takes ownership)
	void setHand(Hand* newHand);

	// Order management
	void addOrder(Order* order);

	// Required methods
	std::vector<Territory*> toDefend(); // Returns territories to be defended
	std::vector<Territory*> toAttack(); // Returns territories to be attacked
	void issueOrder(Card* playedCard,
					Deck* deck); // Creates an order and adds to list

	// Getters
	const std::string &getName() const;
	OrdersList* getOrdersList() const;

	friend std::ostream &operator<<(std::ostream &os, const Player &player);
};
