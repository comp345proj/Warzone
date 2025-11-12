#include "Player.h"
#include "PlayerStrategies/PlayerStrategies.h"
#include <algorithm>
#include <iostream>
#include <utility> // for std::move

//---------------------------Player-------------------------------
Player::Player(const std::string &name, PlayerStrategy* strategy, Hand* hand)
    : name(new std::string(name)),
      hand(hand ? std::make_unique<Hand>(*hand) : std::make_unique<Hand>()),
      ordersList(new OrdersList()), reinforcementPool(new int(0)),
      availableReinforcementPool(0), strategy(strategy),
      hasCheatedThisTurn(false) {
    if (!strategy) {
        this->strategy = new HumanPlayerStrategy();
    }
}

Player::Player(const Player &copiedPlayer)
    : name(new std::string(*copiedPlayer.name)),
      territories(copiedPlayer.territories),
      hand(std::make_unique<Hand>(*copiedPlayer.hand)),
      reinforcementPool(new int(*copiedPlayer.reinforcementPool)),
      availableReinforcementPool(copiedPlayer.availableReinforcementPool),
      strategy(copiedPlayer.strategy),
      hasCheatedThisTurn(copiedPlayer.hasCheatedThisTurn) {
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
        availableReinforcementPool = other.availableReinforcementPool;
        strategy = other.strategy;
        hasCheatedThisTurn = other.hasCheatedThisTurn;
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
        territory->setPlayer(this); // Set the territory's owner
    }
}

void Player::removeTerritory(Territory* territory) {
    territories.erase(
        std::remove(territories.begin(), territories.end(), territory),
        territories.end());
    if (territory && territory->getPlayer() == this) {
        territory->setPlayer(nullptr); // Clear the territory's owner
    }
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
    return strategy ? strategy->toDefend(this) : std::vector<Territory*>();
}

std::vector<Territory*> Player::toAttack() {
    return strategy ? strategy->toAttack(this) : std::vector<Territory*>();
}

void Player::issueOrder(Deck* deck) {
    if (strategy) {
        strategy->issueOrder(this, deck);
    }
}

void Player::issueAdvanceOrder(Territory* from, Territory* to, int numArmies) {
    // No validation at order creation - will be validated during execution
    Advance* advanceOrder = new Advance(this, from, to, numArmies);
    addOrder(advanceOrder);
}

const std::string &Player::getName() const {
    return *name;
}

int Player::getReinforcementPool() const {
    return *reinforcementPool;
}

int Player::getAvailableReinforcementPool() const {
    return availableReinforcementPool;
}

void Player::setReinforcementPool(int amount) {
    *reinforcementPool = amount;
}

void Player::resetAvailableReinforcementPool() {
    availableReinforcementPool = *reinforcementPool;
}

void Player::decrementAvailableReinforcementPool(int amount) {
    availableReinforcementPool -= amount;
}

std::ostream &operator<<(std::ostream &os, const Player &player) {
    os << "Player: " << *player.name << "\n";
    os << "Territories owned: " << player.territories.size() << "\n";
    os << "Cards in hand: " << player.hand->getCards().size() << "\n";
    os << "Orders issued: " << player.ordersList->getOrders().size() << "\n";
    os << "Reinforcement pool: " << *player.reinforcementPool << "\n";
    return os;
}
