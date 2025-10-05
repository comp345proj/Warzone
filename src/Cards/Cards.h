#pragma once
#include "Orders/Orders.h"
#include "Player/Player.h"
#include <memory>
#include <random>
#include <string>
#include <vector>

// Forward declarations
class Player;
class Card;
class Hand;
class Deck;

// Enum for card types
enum class CardType {
	BOMB,
	REINFORCEMENT,
	BLOCKADE,
	AIRLIFT,
	DIPLOMACY,
	UNKNOWN
};

// Card class
class Card {
  public:
	Card(CardType type);
	virtual ~Card();

	// Convert card type to string
	static std::string getTypeString(CardType type);

	// Play the card, create corresponding order, and return card to deck
	void play(Player* player, Deck* deck);

	// Get the type of card
	CardType getType() const;

	// Stream insertion operator
	friend std::ostream &operator<<(std::ostream &os, const Card &card);

  private:
	CardType type;

	// Helper to create appropriate order based on card type
	Order* createOrder() const;
};

// Hand class to manage a player's cards
class Hand {
  public:
	Hand();
	~Hand();

	// Add and remove cards from hand
	void addCard(Card* card);
	void removeCard(Card* card);

	// Get cards in hand
	const std::vector<Card*> &getCards() const;

	// Stream insertion operator
	friend std::ostream &operator<<(std::ostream &os, const Hand &hand);

  private:
	std::vector<Card*> cards;
};

// Deck class to manage all cards
class Deck {
  public:
	Deck();
	~Deck();

	// Initialize deck with cards
	void initialize();

	// Draw a random card from deck
	Card* draw();

	// Return a card to the deck
	void returnCard(Card* card);

	// Get remaining cards count
	size_t size() const;

	// Stream insertion operator
	friend std::ostream &operator<<(std::ostream &os, const Deck &deck);

  private:
	std::vector<Card*> cards;
	std::default_random_engine rng; // Random number generator
};

// Stream insertion operators declarations
std::ostream &operator<<(std::ostream &os, const Card &card);
std::ostream &operator<<(std::ostream &os, const Hand &hand);
std::ostream &operator<<(std::ostream &os, const Deck &deck);