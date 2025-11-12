#include "Orders.h"
#include "Cards/Cards.h"
#include "Player/Player.h"
#include "PlayerStrategies/PlayerStrategies.h"
#include <algorithm>
#include <sstream>

//---------------------------Order (Base class)-------------------------------
Order::Order() : issuingPlayer(nullptr), cardType(CardType::UNKNOWN) {}
Order::~Order() = default;

const std::string &Order::getEffect() const {
    return effect;
}

// Set the effect of the order throughout the game (for logging purposes)
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
    return getEffect();
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
    if (!order.effect.empty()) {
        os << "\nEffect: " << order.effect;
    }
    return os;
}

//---------------------------OrdersList-------------------------------
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
    if (it == orders.end() || newPosition < 0
        || static_cast<size_t>(newPosition) >= orders.size()) {
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

bool Deploy::validate() {
    // Basic parameter validation
    if (!issuingPlayer || !target) {
        setEffect("✗ Deploy: Could not execute order."
                  " Missing issuing player or target territory.");
        return false;
    }

    // Check territory ownership
    if (target->getPlayer() != issuingPlayer) {
        setEffect("✗ Deploy: Could not execute order."
                  " Target territory not owned by issuing player.");
        return false;
    }

    // Check valid army count and reinforcement pool
    if (numArmies <= 0 || numArmies > issuingPlayer->getReinforcementPool()) {
        setEffect("✗ Deploy: Could not execute order."
                  " Invalid number of armies to deploy.");
        return false;
    }
    return true;
}

void Deploy::execute() {
    if (validate()) {
        // Deduct from reinforcement pool and add to territory
        issuingPlayer->setReinforcementPool(
            issuingPlayer->getReinforcementPool() - numArmies);
        target->addArmies(numArmies);
        setEffect("✓ Deploy: Sent " + std::to_string(numArmies) + " armies to "
                  + target->getName() + " (territory now has "
                  + std::to_string(target->getArmies()) + " total armies). "
                  + std::to_string(issuingPlayer->getReinforcementPool())
                  + " armies remaining in reinforcement pool");
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

bool Advance::validate() {
    // Basic parameter validation
    if (!issuingPlayer || !source || !target || numArmies <= 0) {
        setEffect(
            "✗ Advance: Could not execute order."
            " Missing issuing player, source, target, or invalid army count.");
        return false;
    }

    // Source territory must have an owner
    if (!source->getPlayer()) {
        setEffect("✗ Advance: Could not execute order."
                  " Source territory has no owner.");
        return false;
    }

    // Source territory ownership
    if (source->getPlayer() != issuingPlayer) {
        setEffect("✗ Advance: Could not execute order."
                  " Source territory not owned by issuing player.");
        return false;
    }

    // Territory adjacency
    auto neighbors = source->getAdjacentTerritories();
    if (std::find(neighbors.begin(), neighbors.end(), target)
        == neighbors.end()) {
        setEffect("✗ Advance: Could not execute order."
                  " Target territory not adjacent to source territory.");
        return false;
    }

    // Army count
    if (source->getArmies() < numArmies) {
        setEffect("✗ Advance: Could not execute order."
                  " Not enough armies in source territory.");
        return false;
    }

    // Negotiation check (only if target has an owner)
    if (target->getPlayer() && target->getPlayer() != issuingPlayer
        && !canAttack(issuingPlayer, target->getPlayer())) {
        setEffect("✗ Advance: Could not execute order."
                  " Attack not allowed due to negotiation.");
        return false;
    }
    return true;
}

void Advance::execute() {
    if (validate()) {
        std::stringstream effectStream;

        // Move armies from source
        source->removeArmies(numArmies);

        // If target belongs to issuing player, simply move armies
        if (target->getPlayer() && target->getPlayer() == issuingPlayer) {
            target->addArmies(numArmies);
            setEffect("✓ Advance: Moved " + std::to_string(numArmies)
                      + " armies from " + source->getName() + " to "
                      + target->getName());
        }
        // If target has no owner, claim it without battle
        else if (!target->getPlayer()) {
            target->setPlayer(issuingPlayer);
            target->setArmies(numArmies);
            issuingPlayer->addTerritory(target);
            setEffect("✓ Advance: Claimed unoccupied territory "
                      + target->getName() + " with " + std::to_string(numArmies)
                      + " armies");

            // Award card for first conquest
            if (!issuingPlayer->hasConqueredTerritoryThisTurn()) {
                CardType cardTypes[] = {CardType::REINFORCEMENT, CardType::BOMB,
                                        CardType::AIRLIFT, CardType::BLOCKADE,
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
                Player* previousOwner = target->getPlayer();
                target->setPlayer(issuingPlayer);
                target->setArmies(survivingAttackers);
                issuingPlayer->addTerritory(target);
                if (previousOwner) {
                    previousOwner->removeTerritory(target);
                }
                setEffect("✓ Advance: Captured " + target->getName() + " with "
                          + std::to_string(survivingAttackers)
                          + " surviving attackers");

                // Check if defender was Neutral and change strategy
                if (previousOwner && previousOwner->getStrategy()) {
                    if (dynamic_cast<NeutralPlayerStrategy*>(
                            previousOwner->getStrategy())) {
                        previousOwner->setStrategy(
                            new AggressivePlayerStrategy());
                        std::cout << "Neutral player "
                                  << previousOwner->getName()
                                  << " was attacked and became Aggressive!"
                                  << std::endl;
                    }
                }

                if (!issuingPlayer->hasConqueredTerritoryThisTurn()) {
                    CardType cardTypes[] = {CardType::REINFORCEMENT,
                                            CardType::BOMB, CardType::AIRLIFT,
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
                setEffect("✓ Advance: Attack on " + target->getName()
                          + " was executed, but it failed. "
                          + std::to_string(survivingDefenders)
                          + " defenders remaining");
            }
        }
    }
    Notify(this);
}

//---------------------------Bomb-------------------------------
Bomb::Bomb(Player* player, Territory* target) : target(target) {
    description = "Bomb " + target->getName();
    cardType = CardType::BOMB;
    issuingPlayer = player;
}

bool Bomb::validate() {
    // Basic parameter validation
    if (!issuingPlayer || !target) {
        setEffect("✗ Bomb: Could not execute order."
                  " Missing issuing player or target.");
        return false;
    }

    // Target must have an owner
    if (!target->getPlayer()) {
        setEffect("✗ Bomb: Could not execute order."
                  " Target territory has no owner.");
        return false;
    }

    // Can't bomb own territory
    if (target->getPlayer() == issuingPlayer) {
        setEffect("✗ Bomb: Could not execute order."
                  " Cannot bomb own territory.");
        return false;
    }

    // Target must be adjacent to a player's territory
    bool isAdjacent = false;
    for (Territory* territory : issuingPlayer->getTerritories()) {
        auto neighbors = territory->getAdjacentTerritories();
        if (std::find(neighbors.begin(), neighbors.end(), target)
            != neighbors.end()) {
            isAdjacent = true;
            break;
        }
    }
    if (!isAdjacent) {
        setEffect("✗ Bomb: Could not execute order."
                  " Target territory not adjacent to any owned territory.");
    }
    return isAdjacent;
}

void Bomb::execute() {
    if (validate()) {
        int currentArmies = target->getArmies();
        int removedArmies = currentArmies / 2;
        target->removeArmies(removedArmies);
        setEffect("✓ Bomb: Sent explosives to " + target->getName()
                  + " (owned by " + target->getPlayer()->getName()
                  + "), destroying " + std::to_string(removedArmies)
                  + " armies. " + std::to_string(target->getArmies())
                  + " armies remaining.");
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

bool Blockade::validate() {
    // Basic parameter validation
    if (!issuingPlayer || !target) {
        setEffect("✗ Blockade: Could not execute order."
                  " Missing issuing player or target.");
        return false;
    }

    // Target must have an owner
    if (!target->getPlayer()) {
        setEffect("✗ Blockade: Could not execute order."
                  " Target territory has no owner.");
        return false;
    }

    // Must be player's own territory
    if (target->getPlayer() != issuingPlayer) {
        setEffect("✗ Blockade: Could not execute order."
                  " Target territory not owned by issuing player.");
        return false;
    }
    return true;
}

void Blockade::execute() {
    if (validate()) {
        // Double armies and transfer to neutral
        int currentArmies = target->getArmies();
        int newArmies = currentArmies * 2;
        target->setArmies(newArmies);
        Player* previousOwner = target->getPlayer();
        target->setPlayer(getNeutralPlayer());

        // Remove territory from previous owner and add to neutral
        previousOwner->removeTerritory(target);
        getNeutralPlayer()->addTerritory(target);

        setEffect("✓ Blockade: Applied on " + target->getName()
                  + ": Armies doubled from " + std::to_string(currentArmies)
                  + " to " + std::to_string(newArmies)
                  + ". Territory transferred from " + previousOwner->getName()
                  + " to Neutral player.");
    }
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

bool Airlift::validate() {
    // Basic parameter validation
    if (!issuingPlayer || !source || !target || numArmies <= 0) {
        setEffect("✗ Airlift: Could not execute order."
                  " Missing issuing player, source, target, or invalid number "
                  "of armies.");
        return false;
    }

    // Territories must have owners
    if (!source->getPlayer() || !target->getPlayer()) {
        setEffect("✗ Airlift: Could not execute order."
                  " Source or target territory has no owner.");
        return false;
    }

    // Source territory ownership
    if (source->getPlayer() != issuingPlayer) {
        setEffect("✗ Airlift: Could not execute order."
                  " Source territory not owned by issuing player.");
        return false;
    }

    // Target territory ownership
    if (target->getPlayer() != issuingPlayer) {
        setEffect("✗ Airlift: Could not execute order."
                  " Target territory not owned by issuing player.");
        return false;
    }

    // Army count check
    if (source->getArmies() < numArmies) {
        setEffect("✗ Airlift: Could not execute order."
                  " Not enough armies in source territory.");
        return false;
    }
    return true;
}

void Airlift::execute() {
    if (validate()) {
        // Move armies between territories (no adjacency required)
        source->removeArmies(numArmies);
        target->addArmies(numArmies);
        setEffect("✓ Airlift: Sent " + std::to_string(numArmies)
                  + " armies from " + source->getName() + " (now has "
                  + std::to_string(source->getArmies()) + ") to "
                  + target->getName() + " (now has "
                  + std::to_string(target->getArmies()) + ")");
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

bool Negotiate::validate() {
    // Basic parameter validation
    if (!issuingPlayer || !targetPlayer) {
        setEffect(
            "✗ Negotiate: Could not execute order. Missing issuing player "
            "or target player.");
        return false;
    }

    // Can't negotiate with self
    if (issuingPlayer == targetPlayer) {
        setEffect("✗ Negotiate: Could not execute order."
                  " Cannot negotiate with oneself.");
        return false;
    }
    return true;
}

void Negotiate::execute() {
    if (validate()) {
        // Establish peace between players for this turn
        Advance::addNegotiatedPair(issuingPlayer, targetPlayer);
        setEffect(
            "✓ Negotiate: Peace treaty established between "
            + issuingPlayer->getName() + " and " + targetPlayer->getName()
            + ". These players cannot attack each other for the remainder of "
              "this turn.");
    }
    Notify(this);
}

std::string OrdersList::stringToLog() {
    Order* lastOrder = orders.back();
    const std::type_info &type = typeid(*lastOrder);
    std::string typeName = type.name();
    size_t pos = typeName.find_first_not_of("0123456789");
    std::string className = typeName.substr(pos);
    return "Order issued: " + std::string(className) + " from "
        + lastOrder->getPlayer()->getName() + "'s orders list.";
}

std::string Deploy::stringToLog() {
    return getEffect();
}

std::string Advance::stringToLog() {
    return getEffect();
}

std::string Bomb::stringToLog() {
    return getEffect();
}

std::string Blockade::stringToLog() {
    return getEffect();
}

std::string Airlift::stringToLog() {
    return getEffect();
}

std::string Negotiate::stringToLog() {
    return getEffect();
}
