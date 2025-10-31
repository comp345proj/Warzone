#pragma once
#include <iostream>
#include <list>
#include <string>
#include "Observer/LogObserver.h"
#include "Subject/Subject.h"

// Forward declarations
enum class CardType;
class Order;
class OrdersList;
class Player;
class Map;

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
	CardType cardType;   // Type of card that made the order

	// Get the order description and effect
	const std::string &getDescription() const;
	const std::string &getEffect() const;

	// Set the effect after execution
	void setEffect(const std::string &effect);

	std::string description; // Description of the order
	std::string effect;		 // Effect of the order after execution
	bool executed;			 // Flag to track if order was executed

	friend std::ostream &operator<<(std::ostream &os, const Order &order);
};

// Class to manage a list of orders
class OrdersList : public ILoggable, public Subject {
  private:
	std::list<Order*> orders;

  public:
	OrdersList();
	OrdersList(const OrdersList& other);
	~OrdersList();

	// List management methods
	void add(Order* order);
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
  public:
	Deploy();
	bool validate() const override;
	void execute() override;
	// Logging method
	std::string stringToLog() override;
};

class Advance : public Order {
  public:
	Advance();
	bool validate() const override;
	void execute() override;
	// Logging method
	std::string stringToLog() override;
};

class Bomb : public Order {
  public:
	Bomb();
	bool validate() const override;
	void execute() override;
	// Logging method
	std::string stringToLog() override;
};

class Blockade : public Order {
  public:
	Blockade();
	bool validate() const override;
	void execute() override;
	// Logging method
	std::string stringToLog() override;
};

class Airlift : public Order {
  public:
	Airlift();
	bool validate() const override;
	void execute() override;
	// Logging method
	std::string stringToLog() override;
};

class Negotiate : public Order {
  public:
	Negotiate();
	bool validate() const override;
	void execute() override;
	// Logging method
	std::string stringToLog() override;
};

// Stream insertion operators declarations
std::ostream &operator<<(std::ostream &os, const Order &order);
std::ostream &operator<<(std::ostream &os, const OrdersList &ordersList);
