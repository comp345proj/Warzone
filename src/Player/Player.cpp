#include "Player.h"
#include <algorithm>
#include <iostream>
#include <utility> // for std::move

Player::Player(const std::string &name, Hand* hand)
	: name(new std::string(name)),
	  hand(hand ? std::make_unique<Hand>(*hand) : std::make_unique<Hand>()),
	  reinforcementPool(new int(0)) {
	// Ensure ordersList is initialized
	ordersList = new OrdersList();
}

Player::Player(const Player &copiedPlayer)
	: name(new std::string(*copiedPlayer.name)),
	  territories(copiedPlayer.territories),
	  hand(std::make_unique<Hand>(*copiedPlayer.hand)),
	  reinforcementPool(new int(*copiedPlayer.reinforcementPool)) {
	// Deep copy ordersList if present
	if (copiedPlayer.ordersList) {
		ordersList = new OrdersList(*copiedPlayer.ordersList);
	} else {
		ordersList = new OrdersList();
	}
}

Player &Player::operator=(const Player &other) {
	if (this != &other) {
		delete name; // Free existing resource
		delete reinforcementPool;
		name = new std::string(*other.name);
		territories = other.territories;
		hand = std::make_unique<Hand>(*other.hand);
		reinforcementPool = new int(*other.reinforcementPool);
		// Deep copy other's ordersList
		if (ordersList) {
			delete ordersList;
			ordersList = nullptr;
		}
		if (other.ordersList) {
			ordersList = new OrdersList(*other.ordersList);
		} else {
			ordersList = new OrdersList();
		}
	}
	return *this;
}

Player::~Player() {
	delete name;
	delete reinforcementPool;
	if (ordersList) {
		delete ordersList;
		ordersList = nullptr;
	}
}

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

void Player::addOrder(Order* order) {
	if (order) {
		ordersList->addOrder(order);
	}
}

OrdersList* Player::getOrdersList() const {
	return ordersList;
}

std::vector<Territory*> Player::toDefend() {
	std::vector<Territory*> toDefendList = territories;

	// Sort territories by priority (in this case, by number of armies)
	std::sort(
		toDefendList.begin(), toDefendList.end(),
		[](Territory* a, Territory* b) {
			return a->getArmies() <
				   b->getArmies(); // Prioritize territories with fewer armies
		});

	return toDefendList;
}

std::vector<Territory*> Player::toAttack() {
	std::vector<Territory*> toAttackList;
	std::set<Territory*> enemyTerritories;

	// Find all neighboring enemy territories
	for (Territory* myTerritory : territories) {
		const std::vector<Territory*> &neighbors = myTerritory->getNeighbors();
		for (Territory* neighbor : neighbors) {
			// Check if this territory belongs to another player
			bool isEnemyTerritory = true;
			for (Territory* ownedTerritory : territories) {
				if (neighbor == ownedTerritory) {
					isEnemyTerritory = false;
					break;
				}
			}
			if (isEnemyTerritory) {
				enemyTerritories.insert(neighbor);
			}
		}
	}

	// Convert set to vector and sort by priority (in this case, by number of
	// armies)
	toAttackList = std::vector<Territory*>(enemyTerritories.begin(),
										   enemyTerritories.end());
	std::sort(
		toAttackList.begin(), toAttackList.end(),
		[](Territory* a, Territory* b) {
			return a->getArmies() <
				   b->getArmies(); // Prioritize territories with fewer armies
		});

	return toAttackList;
}

void Player::issueOrder(Deck* deck, Card* playedCard) {
	if (getReinforcementPool() > 0) {
		auto territoriesToDefend = toDefend();
		if (!territoriesToDefend.empty()) {
			Territory* target = territoriesToDefend[0];
			int armies = getReinforcementPool();
			Deploy* deployOrder = new Deploy(this, target, armies);
			addOrder(deployOrder);
			setReinforcementPool(0);
			return;
		}
	}

	if (playedCard != nullptr && deck != nullptr) {
		playedCard->play(this, deck);
		return;
	}

	auto defendList = toDefend();
	auto attackList = toAttack();

	if (!defendList.empty() && (attackList.empty() || rand() % 2 == 0)) {
		// Issue a defend order
		Territory* fromTerritory = defendList[rand() % defendList.size()];
		Territory* toTerritory = defendList[rand() % defendList.size()];
		if (fromTerritory != toTerritory && fromTerritory->getArmies() > 0) {
			int armiesToMove = rand() % fromTerritory->getArmies() + 1;
			issueAdvanceOrder(fromTerritory, toTerritory, armiesToMove);
		}
	} else if (!attackList.empty()) {
		// Issue an attack order
		Territory* fromTerritory = defendList[rand() % defendList.size()];
		Territory* toTerritory = attackList[rand() % attackList.size()];
		if (fromTerritory->getArmies() > 0) {
			int armiesToAttack = rand() % fromTerritory->getArmies() + 1;
			issueAdvanceOrder(fromTerritory, toTerritory, armiesToAttack);
		}
	}
}

void Player::issueAdvanceOrder(Territory* from, Territory* to, int numArmies) {
	if (from && to && numArmies > 0) {
		Advance* advanceOrder = new Advance(this, from, to, numArmies);
		addOrder(advanceOrder);
	}
}

const std::string &Player::getName() const {
	return *name;
}

int Player::getReinforcementPool() const {
	return *reinforcementPool;
}

void Player::setReinforcementPool(int amount) {
	*reinforcementPool = amount;
}

std::ostream &operator<<(std::ostream &os, const Player &player) {
	os << "Player: " << *player.name << "\n";
	os << "Territories owned: " << player.territories.size() << "\n";
	os << "Cards in hand: " << player.hand->getCards().size() << "\n";
	os << "Orders issued: " << player.ordersList->getOrders().size() << "\n";
	os << "Reinforcement pool: " << *player.reinforcementPool << "\n";
	return os;
}
