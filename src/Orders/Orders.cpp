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
        return card->getCardType() == type;
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
                case CardType::UNKNOWN:
                    if (dynamic_cast<Deploy*>(*it))
                        orders.push_back(
                            new Deploy(*dynamic_cast<Deploy*>(*it)));
                    else if (dynamic_cast<Advance*>(*it))
                        orders.push_back(
                            new Advance(*dynamic_cast<Advance*>(*it)));
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
            if (player && order->getCardType() != CardType::UNKNOWN) {
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
    description = "Deploy " + std::to_string(numArmies) + " armies to "
        + target->getName();
    cardType = CardType::UNKNOWN;
    issuingPlayer = player;
}

bool Deploy::validate() const {
    // Basic parameter validation
    if (!issuingPlayer || !target) {
        return false;
    }
    // Check territory ownership
    if (target->getPlayer() != issuingPlayer) {
        return false;
    }
    // Check valid army count and reinforcement pool
    if (numArmies <= 0 || numArmies > issuingPlayer->getReinforcementPool()) {
        return false;
    }
    return true;
}

void Deploy::execute() {
    if (!validate()) {
        if (!issuingPlayer || !target) {
            setEffect("Invalid order parameters");
        } else if (target->getPlayer() != issuingPlayer) {
            setEffect("Cannot deploy to " + target->getName()
                      + " - not owned by " + issuingPlayer->getName());
        } else {
            setEffect("Cannot deploy " + std::to_string(numArmies)
                      + " armies - only "
                      + std::to_string(issuingPlayer->getReinforcementPool())
                      + " available in reinforcement pool");
        }
    } else {
        // Deduct from reinforcement pool and add to territory
        issuingPlayer->setReinforcementPool(
            issuingPlayer->getReinforcementPool() - numArmies);
        target->addArmies(numArmies);
        setEffect("Deployed " + std::to_string(numArmies) + " armies to "
                  + target->getName() + " (now has "
                  + std::to_string(target->getArmies()) + " armies). "
                  + std::to_string(issuingPlayer->getReinforcementPool())
                  + " armies remaining in reinforcement pool");
        executed = true;
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
    return negotiatedPairs.find(std::make_pair(attacker, defender))
        == negotiatedPairs.end();
}

Advance::Advance(Player* player,
                 Territory* source,
                 Territory* target,
                 int numArmies)
    : source(source), target(target), numArmies(numArmies) {
    description = "Advance " + std::to_string(numArmies) + " armies from "
        + source->getName() + " to " + target->getName();
    cardType = CardType::UNKNOWN; // Advance orders don't require cards
    issuingPlayer = player;
}

bool Advance::validate() const {
    // Basic parameter validation
    if (!issuingPlayer || !source || !target || numArmies <= 0) {
        return false;
    }
    // Source territory must have an owner
    if (!source->getPlayer()) {
        return false;
    }
    // Source territory ownership
    if (source->getPlayer() != issuingPlayer) {
        return false;
    }
    // Territory adjacency
    auto neighbors = source->getNeighbors();
    if (std::find(neighbors.begin(), neighbors.end(), target)
        == neighbors.end()) {
        return false;
    }
    // Army count
    if (source->getArmies() < numArmies) {
        return false;
    }
    // Negotiation check (only if target has an owner)
    if (target->getPlayer() && target->getPlayer() != issuingPlayer
        && !canAttack(issuingPlayer, target->getPlayer())) {
        return false;
    }
    return true;
}

void Advance::execute() {
    std::stringstream effectStream;

    if (!validate()) {
        Notify(this);
        return;
    }

    // Move armies from source
    source->removeArmies(numArmies);

    // If target belongs to issuing player, simply move armies
    if (target->getPlayer() && target->getPlayer() == issuingPlayer) {
        target->addArmies(numArmies);
        setEffect("Moved " + std::to_string(numArmies) + " armies from "
                  + source->getName() + " to " + target->getName());
    }
    // If target has no owner, claim it without battle
    else if (!target->getPlayer()) {
        target->setPlayer(issuingPlayer);
        target->setArmies(numArmies);
        issuingPlayer->addTerritory(target);
        setEffect("Claimed unoccupied territory " + target->getName() + " with "
                  + std::to_string(numArmies) + " armies");

        // Award card for first conquest
        if (!issuingPlayer->hasConqueredTerritoryThisTurn()) {
            CardType cardTypes[] = {CardType::REINFORCEMENT,
                                    CardType::BOMB,
                                    CardType::AIRLIFT,
                                    CardType::BLOCKADE,
                                    CardType::DIPLOMACY};
            int randomIndex = rand() % 5;
            Card* rewardCard = new Card(cardTypes[randomIndex]);
            issuingPlayer->addCard(rewardCard);
            issuingPlayer->setConqueredTerritoryThisTurn(true);
            effectStream << ". Awarded "
                         << cardTypeToString(cardTypes[randomIndex])
                         << " card for conquest";
            setEffect(getEffect() + effectStream.str());
        }
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
            setEffect("Captured " + target->getName() + " with "
                      + std::to_string(survivingAttackers)
                      + " surviving attackers");

            if (!issuingPlayer->hasConqueredTerritoryThisTurn()) {
                CardType cardTypes[] = {CardType::REINFORCEMENT,
                                        CardType::BOMB,
                                        CardType::AIRLIFT,
                                        CardType::BLOCKADE,
                                        CardType::DIPLOMACY};
                int randomIndex = rand() % 5;
                Card* rewardCard = new Card(cardTypes[randomIndex]);

                issuingPlayer->addCard(rewardCard);
                issuingPlayer->setConqueredTerritoryThisTurn(true);

                effectStream << ". Awarded "
                             << cardTypeToString(cardTypes[randomIndex])
                             << " card for conquest";
                setEffect(getEffect() + effectStream.str());
            }
        } else {
            target->setArmies(survivingDefenders);
            setEffect("Attack on " + target->getName() + " failed. "
                      + std::to_string(survivingDefenders)
                      + " defenders remaining");
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
    // Basic parameter validation
    if (!issuingPlayer || !target) {
        return false;
    }
    // Target must have an owner
    if (!target->getPlayer()) {
        return false;
    }
    // Can't bomb own territory
    if (target->getPlayer() == issuingPlayer) {
        return false;
    }
    // Target must be adjacent to a player's territory
    bool isAdjacent = false;
    for (Territory* territory : issuingPlayer->getTerritories()) {
        auto neighbors = territory->getNeighbors();
        if (std::find(neighbors.begin(), neighbors.end(), target)
            != neighbors.end()) {
            isAdjacent = true;
            break;
        }
    }
    return isAdjacent;
}

void Bomb::execute() {
    if (!validate()) {
        // Return card to player if order is invalid (only if player exists)
        if (issuingPlayer) {
            Card* bombCard = new Card(CardType::BOMB);
            issuingPlayer->addCard(bombCard);
        }

        if (!issuingPlayer || !target) {
            setEffect(
                "Invalid order parameters. BOMB card returned to player.");
        } else if (!target->getPlayer()) {
            setEffect(
                "Cannot bomb " + target->getName()
                + " - territory has no owner. BOMB card returned to player.");
        } else if (target->getPlayer() == issuingPlayer) {
            setEffect("Cannot bomb own territory: " + target->getName()
                      + ". BOMB card returned to player.");
        } else {
            setEffect("Cannot bomb " + target->getName()
                      + " - not adjacent to any owned territory. BOMB card "
                        "returned to player.");
        }
        Notify(this);
        return;
    }
    int currentArmies = target->getArmies();
    int removedArmies = currentArmies / 2;
    target->removeArmies(removedArmies);
    setEffect("Bombed " + target->getName() + " (owned by "
              + target->getPlayer()->getName() + "), destroying "
              + std::to_string(removedArmies) + " armies. "
              + std::to_string(target->getArmies()) + " armies remaining.");
    executed = true;
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
    // Basic parameter validation
    if (!issuingPlayer || !target) {
        return false;
    }
    // Target must have an owner
    if (!target->getPlayer()) {
        return false;
    }
    // Must be player's own territory
    if (target->getPlayer() != issuingPlayer) {
        return false;
    }
    return true;
}

void Blockade::execute() {
    if (!validate()) {
        // Return card to player if order is invalid (only if player exists)
        if (issuingPlayer) {
            Card* blockadeCard = new Card(CardType::BLOCKADE);
            issuingPlayer->addCard(blockadeCard);
        }

        if (!issuingPlayer || !target) {
            setEffect(
                "Invalid order parameters. BLOCKADE card returned to player.");
        } else {
            setEffect("Cannot blockade " + target->getName()
                      + " - not owned by " + issuingPlayer->getName()
                      + ". BLOCKADE card returned to player.");
        }
        Notify(this);
        return;
    }
    // Double armies and transfer to neutral
    int currentArmies = target->getArmies();
    int newArmies = currentArmies * 2;
    target->setArmies(newArmies);
    Player* previousOwner = target->getPlayer();
    target->setPlayer(getNeutralPlayer());

    // Remove territory from previous owner and add to neutral
    previousOwner->removeTerritory(target);
    getNeutralPlayer()->addTerritory(target);

    setEffect("Blockaded " + target->getName() + ": Armies doubled from "
              + std::to_string(currentArmies) + " to "
              + std::to_string(newArmies) + ". Territory transferred from "
              + previousOwner->getName() + " to Neutral player.");
    executed = true;
    Notify(this);
}

//---------------------------Airlift-------------------------------
Airlift::Airlift(Player* player,
                 Territory* source,
                 Territory* target,
                 int numArmies)
    : source(source), target(target), numArmies(numArmies) {
    description = "Airlift " + std::to_string(numArmies) + " armies from "
        + source->getName() + " to " + target->getName();
    cardType = CardType::AIRLIFT;
    issuingPlayer = player;
}

bool Airlift::validate() const {
    // Basic parameter validation
    if (!issuingPlayer || !source || !target || numArmies <= 0) {
        return false;
    }
    // Territories must have owners
    if (!source->getPlayer() || !target->getPlayer()) {
        return false;
    }
    // Source territory ownership
    if (source->getPlayer() != issuingPlayer) {
        return false;
    }
    // Target territory ownership
    if (target->getPlayer() != issuingPlayer) {
        return false;
    }
    // Army count check
    if (source->getArmies() < numArmies) {
        return false;
    }
    return true;
}

void Airlift::execute() {
    if (!validate()) {
        // Return card to player if order is invalid (only if player exists)
        if (issuingPlayer) {
            Card* airliftCard = new Card(CardType::AIRLIFT);
            issuingPlayer->addCard(airliftCard);
        }

        if (!issuingPlayer || !source || !target || numArmies <= 0) {
            setEffect(
                "Invalid order parameters. AIRLIFT card returned to player.");
        } else if (source->getPlayer() != issuingPlayer) {
            setEffect("Cannot airlift from " + source->getName()
                      + " - not owned by " + issuingPlayer->getName()
                      + ". AIRLIFT card returned to player.");
        } else if (target->getPlayer() != issuingPlayer) {
            setEffect("Cannot airlift to " + target->getName()
                      + " - not owned by " + issuingPlayer->getName()
                      + ". AIRLIFT card returned to player.");
        } else {
            setEffect("Not enough armies in " + source->getName() + " ("
                      + std::to_string(source->getArmies()) + " available, "
                      + std::to_string(numArmies)
                      + " requested). AIRLIFT card returned to player.");
        }
        Notify(this);
        return;
    }
    // Move armies between territories (no adjacency required)
    source->removeArmies(numArmies);
    target->addArmies(numArmies);
    setEffect("Airlifted " + std::to_string(numArmies) + " armies from "
              + source->getName() + " (now has "
              + std::to_string(source->getArmies()) + ") to "
              + target->getName() + " (now has "
              + std::to_string(target->getArmies()) + ")");
    executed = true;
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
    // Basic parameter validation
    if (!issuingPlayer || !targetPlayer) {
        return false;
    }
    // Can't negotiate with self
    if (issuingPlayer == targetPlayer) {
        return false;
    }
    return true;
}

void Negotiate::execute() {
    if (!validate()) {
        // Return card to player if order is invalid (only if player exists)
        if (issuingPlayer) {
            Card* diplomacyCard = new Card(CardType::DIPLOMACY);
            issuingPlayer->addCard(diplomacyCard);
        }

        if (!issuingPlayer || !targetPlayer) {
            setEffect(
                "Invalid order parameters. DIPLOMACY card returned to player.");
        } else {
            setEffect("Cannot negotiate with yourself. DIPLOMACY card returned "
                      "to player.");
        }
        Notify(this);
        return;
    }
    // Establish peace between players for this turn
    Advance::addNegotiatedPair(issuingPlayer, targetPlayer);
    setEffect("Peace treaty established between " + issuingPlayer->getName()
              + " and " + targetPlayer->getName()
              + ". These players cannot attack each other for the remainder of "
                "this turn.");
    executed = true;
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
    if (executed) {
        return "Order Execution: " + getEffect();
    }
    return "Order Issued: Deploy by " + issuingPlayer->getName();
}

std::string Advance::stringToLog() {
    if (executed) {
        return "Order Execution: " + getEffect();
    }
    return "Order Issued: Advance by " + issuingPlayer->getName();
}

std::string Bomb::stringToLog() {
    if (executed) {
        return "Order Execution: " + getEffect();
    }
    return "Order Issued: Bomb by " + issuingPlayer->getName();
}

std::string Blockade::stringToLog() {
    if (executed) {
        return "Order Execution: " + getEffect();
    }
    return "Order Issued: Blockade by " + issuingPlayer->getName();
}

std::string Airlift::stringToLog() {
    if (executed) {
        return "Order Execution: " + getEffect();
    }
    return "Order Issued: Airlift by " + issuingPlayer->getName();
}

std::string Negotiate::stringToLog() {
    if (executed) {
        return "Order Execution: " + getEffect();
    }
    return "Order Issued: Negotiate by " + issuingPlayer->getName();
}
