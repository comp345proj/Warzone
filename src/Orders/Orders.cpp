#include "Orders.h"
#include "Cards/Cards.h"
#include "Player/Player.h"
#include <algorithm>
#include <sstream>

//---------------------------Base class-------------------------------
Order::Order()
	: executed(false), issuingPlayer(nullptr), cardType(CardType::UNKNOWN) {}
Order::~Order() = default;

const std::string &Order::getEffect() const {
	return effect;
}

// Set the effect of the order after execution (for logging purposes)
void Order::setEffect(const std::string &effect) {
	this->effect = effect;
}

Player* Order::getPlayer() {
	return issuingPlayer;
}

void Order::setPlayer(Player* player) {
	issuingPlayer = player;
}

CardType Order::getCardType() const {
	return cardType;
}

std::string Order::stringToLog() {
	return "Order executed: " + getEffect();
}

const std::string &Order::getDescription() const {
	return description;
}

// Helper function to validate if player has a specific card type in hand
bool hasCardOfType(const Player* player, CardType type) {
	if (!player)
		return false;

	const auto &cards = player->getCards();
	return std::any_of(cards.begin(), cards.end(), [type](const Card* card) {
		return card->getType() == type;
	});
}

// Operator to print a single order
std::ostream &operator<<(std::ostream &os, const Order &order) {
	os << "Order: " << order.description;
	if (order.executed && !order.effect.empty()) {
		os << "\nEffect: " << order.effect;
	}
	return os;
}

// OrdersList implementation
OrdersList::OrdersList() = default;

OrdersList::OrdersList(const OrdersList &other) {
	if (other.orders.empty()) {
		return;
	} else {
		orders.clear();

		for (auto it = other.orders.begin(); it != other.orders.end(); ++it) {
			switch ((*it)->getCardType()) {
				case CardType::REINFORCEMENT:
					orders.push_back(new Deploy(*dynamic_cast<Deploy*>(*it)));
					break;
				case CardType::UNKNOWN:
					orders.push_back(new Advance(*dynamic_cast<Advance*>(*it)));
					break;
				case CardType::BOMB:
					orders.push_back(new Bomb(*dynamic_cast<Bomb*>(*it)));
					break;
				case CardType::BLOCKADE:
					orders.push_back(
						new Blockade(*dynamic_cast<Blockade*>(*it)));
					break;
				case CardType::AIRLIFT:
					orders.push_back(new Airlift(*dynamic_cast<Airlift*>(*it)));
					break;
				case CardType::DIPLOMACY:
					orders.push_back(
						new Negotiate(*dynamic_cast<Negotiate*>(*it)));
					break;
				default:
					break;
			}
		}
	}
}

OrdersList::~OrdersList() {
	for (Order* order : orders) {
		delete order;
	}
	orders.clear();
}

void OrdersList::addOrder(Order* order) {
	if (order) {
		orders.push_back(order);
	}

	Notify(this);
}

void OrdersList::remove(Order* order) {
	if (order) {
		try {
			orders.remove(order);
			Player* player = order->getPlayer();
			if (player) {
				player->addCard(new Card(order->getCardType()));
			}
			delete order;
		} catch (const std::exception &e) {
			std::cerr << "Exception in OrdersList::remove: " << e.what()
					  << std::endl;
		}
	}
}

// Move order within the list to new position
bool OrdersList::move(Order* order, int newPosition) {
	auto it = std::find(orders.begin(), orders.end(), order);
	if (it == orders.end() || newPosition < 0 || newPosition >= orders.size()) {
		return false;
	}

	orders.remove(order);
	auto newIt = orders.begin();
	std::advance(newIt, newPosition);
	orders.insert(newIt, order);
	return true;
}

const std::list<Order*> &OrdersList::getOrders() const {
	return orders;
}

// Operator to print the entire orders list
std::ostream &operator<<(std::ostream &os, const OrdersList &ordersList) {
	os << "Orders List:" << std::endl;
	for (const Order* order : ordersList.orders) {
		os << *order << std::endl;
	}
	return os;
}

//---------------------------Deploy-------------------------------
Deploy::Deploy(Player* player, Territory* target, int numArmies)
	: target(target), numArmies(numArmies) {
	description = "Deploy " + std::to_string(numArmies) + " armies to " +
				  target->getName();
	cardType = CardType::REINFORCEMENT;
	issuingPlayer = player;
}

bool Deploy::validate() const {
	if (!issuingPlayer || !target)
		return false;
	if (target->getPlayer() != issuingPlayer)
		return false;
	if (numArmies <= 0)
		return false;
	return true;
}

void Deploy::execute() {
	if (validate()) {
		target->addArmies(numArmies);
		setEffect("Deployed " + std::to_string(numArmies) + " armies to " +
				  target->getName());
		executed = true;
	} else {
		setEffect("Deploy order invalid");
	}
	Notify(this);
}

//---------------------------Advance-------------------------------
// Static members for negotiation tracking
std::set<std::pair<Player*, Player*>> negotiatedPairs;

void Advance::addNegotiatedPair(Player* p1, Player* p2) {
	negotiatedPairs.insert(std::make_pair(p1, p2));
	negotiatedPairs.insert(std::make_pair(p2, p1));
}

void Advance::clearNegotiatedPairs() {
	negotiatedPairs.clear();
}

bool Advance::canAttack(Player* attacker, Player* defender) {
	return negotiatedPairs.find(std::make_pair(attacker, defender)) ==
		   negotiatedPairs.end();
}

Advance::Advance(Player* player, Territory* source, Territory* target,
				 int numArmies)
	: source(source), target(target), numArmies(numArmies) {
	description = "Advance " + std::to_string(numArmies) + " armies from " +
				  source->getName() + " to " + target->getName();
	cardType = CardType::UNKNOWN;
	issuingPlayer = player;
}

bool Advance::validate() const {
	if (!issuingPlayer || !source || !target || numArmies <= 0)
		return false;
	if (source->getPlayer() != issuingPlayer)
		return false;

	// Check if target is adjacent
	auto neighbors = source->getNeighbors();
	if (std::find(neighbors.begin(), neighbors.end(), target) ==
		neighbors.end())
		return false;

	// Check if source has enough armies
	if (source->getArmies() < numArmies)
		return false;

	// Check for negotiation block
	if (target->getPlayer() != issuingPlayer &&
		!canAttack(issuingPlayer, target->getPlayer()))
		return false;

	return true;
}

void Advance::execute() {
	if (!validate()) {
		setEffect("Advance order invalid");
		Notify(this);
		return;
	}

	// Move armies from source
	source->removeArmies(numArmies);

	// If target belongs to issuing player, simply move armies
	if (target->getPlayer() == issuingPlayer) {
		target->addArmies(numArmies);
		setEffect("Moved " + std::to_string(numArmies) + " armies from " +
				  source->getName() + " to " + target->getName());
	}
	// Otherwise, initiate battle
	else {
		int attackersKilled = 0;
		int defendersKilled = 0;
		int attackers = numArmies;
		int defenders = target->getArmies();

		// Battle simulation
		for (int i = 0; i < attackers; i++) {
			if (rand() % 100 < 60) // 60% chance to kill defender
				defendersKilled++;
		}
		for (int i = 0; i < defenders; i++) {
			if (rand() % 100 < 70) // 70% chance to kill attacker
				attackersKilled++;
		}

		// Apply casualties
		int survivingAttackers = std::max(0, attackers - attackersKilled);
		int survivingDefenders = std::max(0, defenders - defendersKilled);

		// Determine battle outcome
		if (survivingDefenders == 0 && survivingAttackers > 0) {
			// Territory captured
			target->setPlayer(issuingPlayer);
			target->setArmies(survivingAttackers);
			setEffect("Captured " + target->getName() + " with " +
					  std::to_string(survivingAttackers) +
					  " surviving attackers");

			// Award card for successful conquest
			issuingPlayer->addCard(new Card(CardType::REINFORCEMENT));
		} else {
			target->setArmies(survivingDefenders);
			setEffect("Attack on " + target->getName() + " failed. " +
					  std::to_string(survivingDefenders) +
					  " defenders remaining");
		}
	}
	executed = true;
	Notify(this);
}

//---------------------------Bomb-------------------------------
Bomb::Bomb(Player* player, Territory* target) : target(target) {
	description = "Bomb " + target->getName();
	cardType = CardType::BOMB;
	issuingPlayer = player;
}

bool Bomb::validate() const {
	if (!issuingPlayer || !target)
		return false;
	if (!hasCardOfType(issuingPlayer, CardType::BOMB))
		return false;
	if (target->getPlayer() == issuingPlayer)
		return false;

	// Check if target is adjacent to any of player's territories
	bool isAdjacent = false;
	for (Territory* territory : issuingPlayer->getTerritories()) {
		auto neighbors = territory->getNeighbors();
		if (std::find(neighbors.begin(), neighbors.end(), target) !=
			neighbors.end()) {
			isAdjacent = true;
			break;
		}
	}
	return isAdjacent;
}

void Bomb::execute() {
	if (!validate()) {
		setEffect("Bomb order invalid");
	} else {
		int currentArmies = target->getArmies();
		int removedArmies = currentArmies / 2;
		target->removeArmies(removedArmies);
		setEffect("Bombed " + target->getName() + ", removing " +
				  std::to_string(removedArmies) + " armies");
		executed = true;
	}
	Notify(this);
}

//---------------------------Blockade-------------------------------
Player* Blockade::neutralPlayer = nullptr;

Player* Blockade::getNeutralPlayer() {
	if (!neutralPlayer) {
		neutralPlayer = new Player("Neutral");
	}
	return neutralPlayer;
}

Blockade::Blockade(Player* player, Territory* target) : target(target) {
	description = "Blockade " + target->getName();
	cardType = CardType::BLOCKADE;
	issuingPlayer = player;
}

bool Blockade::validate() const {
	if (!issuingPlayer || !target)
		return false;
	if (!hasCardOfType(issuingPlayer, CardType::BLOCKADE))
		return false;
	return target->getPlayer() == issuingPlayer;
}

void Blockade::execute() {
	if (!validate()) {
		setEffect("Blockade order invalid");
	} else {
		// Double armies
		int currentArmies = target->getArmies();
		target->setArmies(currentArmies * 2);

		// Transfer to neutral player
		target->setPlayer(getNeutralPlayer());

		setEffect("Blockaded " + target->getName() + ", doubling armies to " +
				  std::to_string(currentArmies * 2) +
				  " and transferring to neutral player");
		executed = true;
	}
	Notify(this);
}

//---------------------------Airlift-------------------------------
Airlift::Airlift(Player* player, Territory* source, Territory* target,
				 int numArmies)
	: source(source), target(target), numArmies(numArmies) {
	description = "Airlift " + std::to_string(numArmies) + " armies from " +
				  source->getName() + " to " + target->getName();
	cardType = CardType::AIRLIFT;
	issuingPlayer = player;
}

bool Airlift::validate() const {
	if (!issuingPlayer || !source || !target || numArmies <= 0)
		return false;
	if (!hasCardOfType(issuingPlayer, CardType::AIRLIFT))
		return false;
	if (source->getPlayer() != issuingPlayer ||
		target->getPlayer() != issuingPlayer)
		return false;
	return source->getArmies() >= numArmies;
}

void Airlift::execute() {
	if (!validate()) {
		setEffect("Airlift order invalid");
	} else {
		source->removeArmies(numArmies);
		target->addArmies(numArmies);
		setEffect("Airlifted " + std::to_string(numArmies) + " armies from " +
				  source->getName() + " to " + target->getName());
		executed = true;
	}
	Notify(this);
}

//---------------------------Negotiate-------------------------------
Negotiate::Negotiate(Player* player, Player* targetPlayer)
	: targetPlayer(targetPlayer) {
	description = "Negotiate with " + targetPlayer->getName();
	cardType = CardType::DIPLOMACY;
	issuingPlayer = player;
}

bool Negotiate::validate() const {
	if (!issuingPlayer || !targetPlayer)
		return false;
	if (!hasCardOfType(issuingPlayer, CardType::DIPLOMACY))
		return false;
	return issuingPlayer != targetPlayer;
}

void Negotiate::execute() {
	if (!validate()) {
		setEffect("Negotiate order invalid");
	} else {
		// Add both players to negotiated pairs
		Advance::addNegotiatedPair(issuingPlayer, targetPlayer);
		setEffect("Negotiated peace between " + issuingPlayer->getName() +
				  " and " + targetPlayer->getName());
		executed = true;
	}
	Notify(this);
}

std::string OrdersList::stringToLog() {
	Order* lastOrder = orders.back();
	const std::type_info &type = typeid(*lastOrder);
	std::string typeName = type.name();
	size_t pos = typeName.find_first_not_of("0123456789");
	std::string className = typeName.substr(pos);
	return "Order Issued: " + std::string(className);
}

std::string Deploy::stringToLog() {
	return "Deploy order issued by " + issuingPlayer->getName();
}

std::string Advance::stringToLog() {
	return "Advance order issued by " + issuingPlayer->getName();
}

std::string Bomb::stringToLog() {
	return "Bomb order issued by " + issuingPlayer->getName();
}

std::string Blockade::stringToLog() {
	return "Blockade order issued by " + issuingPlayer->getName();
}

std::string Airlift::stringToLog() {
	return "Airlift order issued by " + issuingPlayer->getName();
}

std::string Negotiate::stringToLog() {
	return "Negotiate order issued by " + issuingPlayer->getName();
}
