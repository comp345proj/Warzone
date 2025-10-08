#include "Player.h"
#include <algorithm>
#include <iostream>
#include <utility> // for std::move

Player::Player(const std::string &name, Hand* hand)
	: name(new std::string(name)),
	  hand(hand ? std::make_unique<Hand>(*hand) : std::make_unique<Hand>()) {}

// Copy constructor (deep copy)
Player::Player(const Player &copiedPlayer)
	: name(new std::string(*copiedPlayer.name)), territories(copiedPlayer.territories),
	  hand(std::make_unique<Hand>(*copiedPlayer.hand)),
	  ordersList(copiedPlayer.ordersList) {}

// Copy assignment (deep copy)
Player &Player::operator=(const Player &other) {
	if (this != &other) {
		delete name; // Free existing resource
		name = new std::string(*other.name);
		territories = other.territories;
		hand = std::make_unique<Hand>(*other.hand);
		ordersList = other.ordersList;
	}
	return *this;
}

// Destructor (no manual delete needed)
Player::~Player() {
	delete name;
}

// Territory management
void Player::addTerritory(Territory* territory) {
	if (territory) {
		territories.push_back(territory);
	}
}

void Player::removeTerritory(Territory* territory) {
	territories.erase(
		std::remove(territories.begin(), territories.end(), territory),
		territories.end());
}

const std::vector<Territory*> &Player::getTerritories() const {
	return territories;
}

// Card management
void Player::addCard(Card* card) {
	if (card && hand) {
		hand->addCard(card);
	}
}

void Player::removeCard(Card* card) {
	if (card && hand) {
		hand->removeCard(card);
	}
}

const std::vector<Card*> &Player::getCards() const {
	return hand->getCards();
}

Hand* Player::getHand() const {
	return hand.get();
}

void Player::setHand(Hand* newHand) {
	if (newHand) {
		hand = std::make_unique<Hand>(*newHand);
	}
}

// Order management
void Player::addOrder(Order* order) {
	if (order) {
		ordersList.push_back(order);
	}
}

const std::vector<Order*> &Player::getOrders() const {
	return ordersList;
}

// Required methods
std::vector<Territory*> Player::toDefend() {
	return territories;
}

std::vector<Territory*> Player::toAttack() {
	return {};
}

void Player::issueOrder(Card* playedCard, Deck* deck) {
	playedCard->play(this, deck);
}

// Getters
const std::string &Player::getName() const {
	return *name;
}

std::ostream &operator<<(std::ostream &os, const Player &player) {
	os << "Player: " << *player.name << "\n";
	os << "Territories owned: " << player.territories.size() << "\n";
	os << "Cards in hand: " << player.hand->getCards().size() << "\n";
	os << "Orders issued: " << player.ordersList.size();
	return os;
}
