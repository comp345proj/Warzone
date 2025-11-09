#pragma once
#include "GameTypes/GameTypes.h"
#include "Observer/LogObserver.h"
#include "Subject/Subject.h"
#include <iostream>
#include <list>
#include <string>

// Forward declarations
class Order;
class OrdersList;
class Player;
class Map;
class Territory;

// Abstract base class for all orders
class Order : public ILoggable, public Subject {
  public:
    Order();
    virtual ~Order();

    // Set issuing player
    void setPlayer(Player* player);

    // Get the issuing player
    Player* getPlayer();

    // Get the card type associated with the order
    CardType getCardType() const;

    // Pure virtual methods that must be implemented by derived classes
    virtual bool validate() const = 0;
    virtual void execute() = 0;

    // Logging method
    std::string stringToLog() override;

  protected:
    Player* issuingPlayer; // Player who issued the order
    CardType cardType;     // Type of card that made the order

    // Get the order description and effect
    const std::string &getDescription() const;
    const std::string &getEffect() const;

    // Set the effect after execution
    void setEffect(const std::string &effect);

    std::string description; // Description of the order
    std::string effect;      // Effect of the order after execution
    bool executed;           // Flag to track if order was executed

    friend std::ostream &operator<<(std::ostream &os, const Order &order);
};

// Class to manage a list of orders
class OrdersList : public ILoggable, public Subject {
  private:
    std::list<Order*> orders;

  public:
    OrdersList();
    OrdersList(const OrdersList &other);
    ~OrdersList();

    // List management methods
    void addOrder(Order* order);
    void remove(Order* order);
    bool move(Order* order, int newPosition);

    // Logging method
    std::string stringToLog() override;

    // Get the list of orders
    const std::list<Order*> &getOrders() const;

    // Stream operator to print the list
    friend std::ostream &operator<<(std::ostream &os,
                                    const OrdersList &ordersList);
};

// Concrete Order classes (sub-classes of Order)
class Deploy : public Order {
  private:
    Territory* target;
    int numArmies;

  public:
    Deploy(Player* player, Territory* target, int numArmies);
    bool validate() const override;
    void execute() override;
    std::string stringToLog() override;
};

class Advance : public Order {
  private:
    Territory* source;
    Territory* target;
    int numArmies;
    static bool canAttack(Player* attacker,
                          Player* defender); // For negotiate validation

  public:
    Advance(Player* player,
            Territory* source,
            Territory* target,
            int numArmies);
    bool validate() const override;
    void execute() override;
    std::string stringToLog() override;

    // Static method to track negotiated pairs
    static void addNegotiatedPair(Player* p1, Player* p2);
    static void clearNegotiatedPairs();
};

class Bomb : public Order {
  private:
    Territory* target;

  public:
    Bomb(Player* player, Territory* target);
    bool validate() const override;
    void execute() override;
    std::string stringToLog() override;
};

class Blockade : public Order {
  private:
    Territory* target;
    static Player* neutralPlayer; // Singleton neutral player

  public:
    Blockade(Player* player, Territory* target);
    bool validate() const override;
    void execute() override;
    std::string stringToLog() override;

    // Static method to get/create neutral player
    static Player* getNeutralPlayer();
};

class Airlift : public Order {
  private:
    Territory* source;
    Territory* target;
    int numArmies;

  public:
    Airlift(Player* player,
            Territory* source,
            Territory* target,
            int numArmies);
    bool validate() const override;
    void execute() override;
    std::string stringToLog() override;
};

class Negotiate : public Order {
  private:
    Player* targetPlayer;

  public:
    Negotiate(Player* player, Player* targetPlayer);
    bool validate() const override;
    void execute() override;
    std::string stringToLog() override;
};

// Stream insertion operators declarations
std::ostream &operator<<(std::ostream &os, const Order &order);
std::ostream &operator<<(std::ostream &os, const OrdersList &ordersList);
