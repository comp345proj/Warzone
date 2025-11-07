#include "Cards.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

//---------------------------Card-------------------------------
Card::Card(CardType type) : type(type) {}
Card::~Card() = default;

// Make order based on card type
Order* Card::createOrder() const {
	switch (type) {
	case CardType::BOMB:
		return new Bomb();
	case CardType::REINFORCEMENT:
		return new Deploy();
	case CardType::BLOCKADE:
		return new Blockade();
	case CardType::AIRLIFT:
		return new Airlift();
	case CardType::DIPLOMACY:
		return new Negotiate();
	default:
		return nullptr;
	}
}

// Plays a card, creates corresponding order, and returns card to deck
// atomically
void Card::play(Player* player, Deck* deck) {
	if (!player || !deck) {
		std::cout << "Invalid player or deck" << std::endl;
		return;
	}

	// Create corresponding order
	Order* order = createOrder();
	if (order) {
		std::cout << "Created " << getTypeString(type) << " order" << std::endl;

		player->removeCard(this); // Remove card from player's hand
		player->addOrder(order);  // Add order to player's order list
	}

	// Return card to deck
	deck->returnCard(this);
}

std::string Card::getTypeString(CardType type) {
	switch (type) {
	case CardType::BOMB:
		return "Bomb";
	case CardType::REINFORCEMENT:
		return "Reinforcement";
	case CardType::BLOCKADE:
		return "Blockade";
	case CardType::AIRLIFT:
		return "Airlift";
	case CardType::DIPLOMACY:
		return "Diplomacy";
	default:
		return "Unknown";
	}
}

CardType Card::getType() const {
	return type;
}

std::ostream &operator<<(std::ostream &os, const Card &card) {
	os << Card::getTypeString(card.getType());
	return os;
}

//---------------------------Hand-------------------------------
Hand::Hand() = default;

Hand::~Hand() {
	// Cards are owned by the deck, so we don't delete them here
	cards.clear();
}

void Hand::addCard(Card* card) {
	if (card) {
		cards.push_back(card);
	}
}

void Hand::removeCard(Card* card) {
	cards.erase(std::remove(cards.begin(), cards.end(), card), cards.end());
}

const std::vector<Card*> &Hand::getCards() const {
	return cards;
}

std::ostream &operator<<(std::ostream &os, const Hand &hand) {
	os << "Hand contains " << hand.cards.size() << " cards:" << std::endl;
	for (const Card* card : hand.cards) {
		os << "- " << *card << std::endl;
	}
	return os;
}

//---------------------------Deck-------------------------------
Deck::Deck()
	: rng(std::chrono::system_clock::now().time_since_epoch().count()) {
	initialize();
}

Deck::Deck(const Deck &other)
	: rng(std::chrono::system_clock::now().time_since_epoch().count()) {
	// Deep copy of cards
	for (Card* card : other.cards) {
		cards.push_back(new Card(card->getType()));
	}
}

Deck &Deck::operator=(const Deck &other) {
	if (this != &other) {
		// Clear existing cards
		for (Card* card : cards) {
			delete card;
		}
		cards.clear();

		// Deep copy of cards
		for (Card* card : other.cards) {
			cards.push_back(new Card(card->getType()));
		}
	}
	return *this;
}

Deck::~Deck() {
	for (Card* card : cards) {
		delete card;
	}
	cards.clear();
}

void Deck::initialize() {
	// Clear existing cards
	for (Card* card : cards) {
		delete card;
	}
	cards.clear();

	// Create cards of each type (multiple copies for game balance)
	const int numCopies = 5; // Number of copies of each card type

	for (int i = 0; i < numCopies; ++i) {
		cards.push_back(new Card(CardType::BOMB));
		cards.push_back(new Card(CardType::REINFORCEMENT));
		cards.push_back(new Card(CardType::BLOCKADE));
		cards.push_back(new Card(CardType::AIRLIFT));
		cards.push_back(new Card(CardType::DIPLOMACY));
	}

	// Shuffle the deck
	std::shuffle(cards.begin(), cards.end(), rng);
}

Card* Deck::draw() {
	if (cards.empty()) {
		return nullptr;
	}

	// Take the top card
	Card* card = cards.back();
	cards.pop_back();
	return card;
}

void Deck::returnCard(Card* card) {
	if (card) {
		cards.push_back(card);
		// Shuffle the deck after returning a card
		std::shuffle(cards.begin(), cards.end(), rng);
	}
}

size_t Deck::size() const {
	return cards.size();
}

std::ostream &operator<<(std::ostream &os, const Deck &deck) {
	os << "Deck contains " << deck.size() << " cards." << std::endl;
	return os;
}