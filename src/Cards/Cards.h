#pragma once
#include "GameTypes/GameTypes.h"
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

// Represents a playing card
class Card {
  private:
    CardType cardType;

  public:
    Card(CardType type);
    virtual ~Card();

    // Play the card, create corresponding order, and return card to deck
    bool play(Player* player, Deck* deck);

    // Get the type of card
    CardType getCardType() const;

    // Stream insertion operator
    friend std::ostream &operator<<(std::ostream &os, const Card &card);
};

// A player's hand of cards
class Hand {
  private:
    std::vector<Card*> cards;

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
};

// Game's deck to manage all cards
class Deck {
  private:
    std::vector<Card*> cards;
    std::default_random_engine rng; // Random number generator

  public:
    Deck();
    Deck(const Deck &other);
    Deck &operator=(const Deck &other);
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
};