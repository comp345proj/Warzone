#include "Cards.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

//---------------------------Card-------------------------------
Card::Card(CardType type) : type(type) {}
Card::~Card() = default;

// Play card >> create order >> return card to deck
bool Card::play(Player* player, Deck* deck) {
	if (!player || !deck) {
		std::cout << "Invalid player or deck" << std::endl;
		return false;
	}

	// Get territories for the order
	auto playerTerritories = player->getTerritories();
	if (playerTerritories.empty()) {
		std::cout << "Player has no territories" << std::endl;
		return false;
	}

	// Create order based on card type with appropriate parameters
	Order* order = nullptr;
	switch (type) {
		case CardType::BOMB: {

			// Find first adjacent enemy territory
			Territory* target = nullptr;
			for (Territory* t : playerTerritories[0]->getNeighbors()) {
				if (t->getPlayer() != player) {
					target = t;
					break;
				}
			}
			if (target) {
				order = new Bomb(player, target);
			}
			break;
		}
		case CardType::REINFORCEMENT: {
			// Deploy to first owned territory
			order =
				new Deploy(player, playerTerritories[0], 5); // Default 5 armies
			break;
		}
		case CardType::BLOCKADE: {
			order = new Blockade(player, playerTerritories[0]);
			break;
		}
		case CardType::AIRLIFT: {
			if (playerTerritories.size() >= 2) {
				order = new Airlift(player, playerTerritories[0],
									playerTerritories[1], 3);
			}
			break;
		}
		case CardType::DIPLOMACY: {
			// Find a player that owns an adjacent territory
			Player* targetPlayer = nullptr;
			for (Territory* t : playerTerritories[0]->getNeighbors()) {
				if (t->getPlayer() != player) {
					targetPlayer = t->getPlayer();
					break;
				}
			}
			if (targetPlayer) {
				order = new Negotiate(player, targetPlayer);
			}
			break;
		}
	}

	if (order) {
		std::cout << "Created " << cardTypeToString(type) << " order" << std::endl;
		player->removeCard(this);
		player->addOrder(order);
	} else {
		std::cout << "Could not create order for " << cardTypeToString(type)
				  << std::endl;
		return false;
	}

	// Return card to deck
	deck->returnCard(this);
	return true;
}

CardType Card::getType() const {
	return type;
}

std::ostream &operator<<(std::ostream &os, const Card &card) {
	os << cardTypeToString(card.getType());
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