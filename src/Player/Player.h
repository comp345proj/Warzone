#pragma once
#include "Cards/Cards.h"
#include "Map/Map.h"
#include "Orders/Orders.h"
#include <string>
#include <vector>

// Forward declarations
class Territory;
class Card;
class Hand;
class Deck;
class OrdersList;
class PlayerStrategy;

// A player in the game
class Player {
  private:
    std::string* name;
    std::vector<Territory*> territories;
    std::unique_ptr<Hand> hand;
    OrdersList* ordersList = nullptr;
    int* reinforcementPool; // Actual pool, modified only during execution
    int availableReinforcementPool; // Available pool for issuing orders
    bool hasReceivedCardThisTurn = false;
    PlayerStrategy* strategy;        // Strategy for behavior
    bool hasCheatedThisTurn = false; // For Cheater strategy

  public:
    bool hasConqueredTerritoryThisTurn() const {
        return hasReceivedCardThisTurn;
    }
    void setConqueredTerritoryThisTurn(bool value) {
        hasReceivedCardThisTurn = value;
    }
    Player(const std::string &name,
           PlayerStrategy* strategy = new HumanPlayerStrategy(),
           Hand* hand = nullptr);
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

    // Strategy methods (delegated)
    std::vector<Territory*>
    toDefend(); // Returns territories to be defended in priority
    std::vector<Territory*>
    toAttack(); // Returns neighboring territories to be attacked in priority

    // Order issuing (delegated)
    void issueOrder(Deck* deck);
    void issueAdvanceOrder(Territory* from, Territory* to, int numArmies);

    // Strategy management
    void setStrategy(PlayerStrategy* strat) { strategy = strat; }
    PlayerStrategy* getStrategy() const { return strategy; }
    bool getHasCheatedThisTurn() const { return hasCheatedThisTurn; }
    void setHasCheatedThisTurn(bool value) { hasCheatedThisTurn = value; }

    // Getters
    const std::string &getName() const;
    OrdersList* getOrdersList() const;
    int getReinforcementPool() const;
    int getAvailableReinforcementPool() const;

    // Setters
    void setReinforcementPool(int amount);
    void
    resetAvailableReinforcementPool();

    friend std::ostream &operator<<(std::ostream &os, const Player &player);
};
