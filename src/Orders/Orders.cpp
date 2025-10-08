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

OrdersList::OrdersList(const OrdersList& other) {
	if (other.orders.empty())
	{
		return;
	} else {
		this->orders.clear();

		for (auto it = other.orders.begin(); it != other.orders.end(); ++it)
		{
			switch ((*it)->getCardType())
			{
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
				orders.push_back(new Blockade(*dynamic_cast<Blockade*>(*it)));
				break;
			case CardType::AIRLIFT:
				orders.push_back(new Airlift(*dynamic_cast<Airlift*>(*it)));
				break;
			case CardType::DIPLOMACY:
				orders.push_back(new Negotiate(*dynamic_cast<Negotiate*>(*it)));
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

void OrdersList::add(Order* order) {
	if (order) {
		orders.push_back(order);
	}
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
		} catch (const std::exception& e) {
			std::cerr << "Exception in OrdersList::remove: " << e.what() << std::endl;
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
Deploy::Deploy() {
	description = "Deploy armies to territory";
	cardType = CardType::REINFORCEMENT;
}

bool Deploy::validate() const {
	if (!issuingPlayer)
		return false;
	return hasCardOfType(issuingPlayer, CardType::REINFORCEMENT);
}

void Deploy::execute() {
	if (validate()) {
		// TODO: Implement proper execution
		setEffect("Deployed armies to territory");
		executed = true;
	}
}

//---------------------------Advance-------------------------------
Advance::Advance() {
	description = "Advance armies to adjacent territory";
	cardType = CardType::UNKNOWN;
}

bool Advance::validate() const {
	if (!issuingPlayer)
		return false;
	return hasCardOfType(issuingPlayer, CardType::UNKNOWN);
}

void Advance::execute() {
	if (validate()) {
		// TODO: Implement proper execution
		setEffect("Advanced armies to adjacent territory");
		executed = true;
	}
}

//---------------------------Bomb-------------------------------
Bomb::Bomb() {
	description = "Bomb enemy territory";
	cardType = CardType::BOMB;
}

bool Bomb::validate() const {
	if (!issuingPlayer)
		return false;
	return hasCardOfType(issuingPlayer, CardType::BOMB);
}

void Bomb::execute() {
	if (validate()) {
		// TODO: Implement proper execution
		setEffect("Bombed enemy territory");
		executed = true;
	}
}

//---------------------------Blockade-------------------------------
Blockade::Blockade() {
	description = "Blockade territory";
	cardType = CardType::BLOCKADE;
}

bool Blockade::validate() const {
	if (!issuingPlayer)
		return false;
	return hasCardOfType(issuingPlayer, CardType::BLOCKADE);
}

void Blockade::execute() {
	if (validate()) {
		// TODO: Implement proper execution
		setEffect("Blockaded territory");
		executed = true;
	}
}

//---------------------------Airlift-------------------------------
Airlift::Airlift() {
	description = "Airlift armies between territories";
	cardType = CardType::AIRLIFT;
}

bool Airlift::validate() const {
	if (!issuingPlayer)
		return false;
	return hasCardOfType(issuingPlayer, CardType::AIRLIFT);
}

void Airlift::execute() {
	if (validate()) {
		// TODO: Implement proper execution
		setEffect("Airlifted armies between territories");
		executed = true;
	}
}

//---------------------------Negotiate-------------------------------
Negotiate::Negotiate() {
	description = "Negotiate with enemy player";
	cardType = CardType::DIPLOMACY;
}

bool Negotiate::validate() const {
	if (!issuingPlayer)
		return false;
	return hasCardOfType(issuingPlayer, CardType::DIPLOMACY);
}

void Negotiate::execute() {
	if (validate()) {
		// TODO: Implement proper execution
		setEffect("Negotiated peace with enemy player");
		executed = true;
	}
}
