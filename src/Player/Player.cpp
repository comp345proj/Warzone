#include "Player.h"
#include <iostream>

Player::Player(const std::string &name) : name(name) {}

Player::Player(const Player &other)
	: name(other.name), territories(other.territories), hand(other.hand),
	  ordersList(other.ordersList) {}

Player &Player::operator=(const Player &other) {
	if (this != &other) {
		name = other.name;
		territories = other.territories;
		hand = other.hand;
		ordersList = other.ordersList;
	}
	return *this;
}

Player::~Player() = default;

std::vector<Territory*> Player::toDefend() {
	// TODO: Implement logic to determine territories to defend
	return std::vector<Territory*>();
}

std::vector<Territory*> Player::toAttack() {
	// TODO: Implement logic to determine territories to attack
	return std::vector<Territory*>();
}

void Player::issueOrder() {
	// TODO: Implement order creation logic
}

std::ostream &operator<<(std::ostream &os, const Player &player) {
	os << "Player: " << player.name;
	return os;
}
