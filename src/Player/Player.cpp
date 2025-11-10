#include "Player.h"
#include <algorithm>
#include <iostream>
#include <utility> // for std::move

//---------------------------Player-------------------------------
Player::Player(const std::string &name, Hand* hand)
    : name(new std::string(name)),
      hand(hand ? std::make_unique<Hand>(*hand) : std::make_unique<Hand>()),
      reinforcementPool(new int(0)), availableReinforcementPool(0) {
    // Ensure ordersList is initialized
    ordersList = new OrdersList();
}

Player::Player(const Player &copiedPlayer)
    : name(new std::string(*copiedPlayer.name)),
      territories(copiedPlayer.territories),
      hand(std::make_unique<Hand>(*copiedPlayer.hand)),
      reinforcementPool(new int(*copiedPlayer.reinforcementPool)),
      availableReinforcementPool(copiedPlayer.availableReinforcementPool) {
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
    std::vector<Territory*> toDefendList = territories;

    // Sort territories by priority (in this case, by number of armies)
    std::sort(
        toDefendList.begin(),
        toDefendList.end(),
        [](Territory* a, Territory* b) {
            return a->getArmies()
                < b->getArmies(); // Prioritize territories with fewer armies
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
        toAttackList.begin(),
        toAttackList.end(),
        [](Territory* a, Territory* b) {
            return a->getArmies()
                < b->getArmies(); // Prioritize territories with fewer armies
        });

    return toAttackList;
}

void Player::issueOrder(Deck* deck) {
    std::cout << "\n=== Player " << getName()
              << "'s Issue Order Phase ===" << std::endl;

    // PHASE 1: FORCED DEPLOYMENT - Must empty reinforcement pool first
    while (availableReinforcementPool > 0) {
        std::cout << "\n[DEPLOY PHASE - MANDATORY]" << std::endl;
        std::cout << "You have " << availableReinforcementPool
                  << " armies in your reinforcement pool that MUST be deployed."
                  << std::endl;

        auto territoriesToDefend = toDefend();
        if (territoriesToDefend.empty()) {
            std::cout << "ERROR: No territories to deploy to!" << std::endl;
            break;
        }

        std::cout << "\nAvailable territories to deploy armies:" << std::endl;
        for (size_t i = 0; i < territoriesToDefend.size(); ++i) {
            std::cout << i + 1 << ". " << territoriesToDefend[i]->getName()
                      << " (Current armies: "
                      << territoriesToDefend[i]->getArmies() << ")"
                      << std::endl;
        }

        std::cout << "\nChoose territory number to deploy (1-"
                  << territoriesToDefend.size() << "): ";
        int choice;
        while (!(std::cin >> choice) || choice < 1
               || choice > static_cast<int>(territoriesToDefend.size())) {
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
            }
            std::cout << "Invalid input. Please enter a number between 1 and "
                      << territoriesToDefend.size() << ": ";
        }

        Territory* target = territoriesToDefend[choice - 1];
        std::cout << "How many armies to deploy (1-"
                  << availableReinforcementPool << "): ";
        int armies;
        while (!(std::cin >> armies) || armies < 1
               || armies > availableReinforcementPool) {
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
            }
            std::cout << "Invalid input. Please enter a number between 1 and "
                      << availableReinforcementPool << ": ";
        }

        Deploy* deployOrder = new Deploy(this, target, armies);
        addOrder(deployOrder);
        availableReinforcementPool -=
            armies; // Deduct from available, not actual
        std::cout << "✓ Deploy order created: " << armies << " armies to "
                  << target->getName() << std::endl;

        if (availableReinforcementPool > 0) {
            std::cout << "\nYou still have " << getReinforcementPool()
                      << " armies to deploy." << std::endl;
        } else {
            std::cout << "\n✓ All reinforcement armies have been deployed!"
                      << std::endl;
        }
    }

    // PHASE 2: OPTIONAL ORDERS - Advance and/or Card orders
    std::cout << "\n[OPTIONAL ORDERS PHASE]" << std::endl;
    std::cout << "You may now issue Advance orders and/or play Cards."
              << std::endl;

    bool continueIssuing = true;
    while (continueIssuing) {
        // Get territory lists
        auto defendList = toDefend();
        auto attackList = toAttack();

        std::cout << "\n--- Choose an action ---" << std::endl;
        std::cout << "1. Issue Advance order (move/attack)" << std::endl;
        std::cout << "2. Play a Card" << std::endl;
        std::cout << "3. End turn" << std::endl;
        std::cout << "Choice: ";

        int mainChoice;
        while (!(std::cin >> mainChoice) || mainChoice < 1 || mainChoice > 3) {
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
            }
            std::cout << "Invalid input. Please enter 1, 2, or 3: ";
        }

        switch (mainChoice) {
            case 1: { // Advance Order
                std::cout << "\n[ADVANCE ORDER]" << std::endl;
                std::cout << "Choose action:" << std::endl;
                std::cout << "1. Move armies between your territories (defend)"
                          << std::endl;
                std::cout << "2. Attack enemy territory" << std::endl;
                std::cout << "Choice: ";

                int advanceChoice;
                while (!(std::cin >> advanceChoice) || advanceChoice < 1
                       || advanceChoice > 2) {
                    if (std::cin.fail()) {
                        std::cin.clear();
                        std::cin.ignore(
                            std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    std::cout << "✗ Invalid input. Please enter 1 or 2: ";
                }

                if (advanceChoice == 1) {
                    // Move between own territories
                    if (defendList.size() < 2) {
                        std::cout << "✗ You need at least 2 territories to "
                                     "move armies."
                                  << std::endl;
                        std::cout << "✗ Advance order NOT created."
                                  << std::endl;
                        break;
                    }

                    std::cout << "\nYour territories (to defend):" << std::endl;
                    for (size_t i = 0; i < defendList.size(); ++i) {
                        std::cout << i + 1 << ". " << defendList[i]->getName()
                                  << " (Armies: " << defendList[i]->getArmies()
                                  << ")" << std::endl;
                    }

                    std::cout << "\nChoose source territory (1-"
                              << defendList.size() << "): ";
                    int sourceChoice;
                    while (!(std::cin >> sourceChoice) || sourceChoice < 1
                           || sourceChoice
                               > static_cast<int>(defendList.size())) {
                        if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(
                                std::numeric_limits<std::streamsize>::max(),
                                '\n');
                        }
                        std::cout << "✗ Invalid input. Please enter a number "
                                     "between 1 and "
                                  << defendList.size() << ": ";
                    }

                    Territory* source = defendList[sourceChoice - 1];

                    std::cout << "Choose destination territory (1-"
                              << defendList.size() << "): ";
                    int destChoice;
                    while (!(std::cin >> destChoice) || destChoice < 1
                           || destChoice
                               > static_cast<int>(defendList.size())) {
                        if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(
                                std::numeric_limits<std::streamsize>::max(),
                                '\n');
                        }
                        std::cout << "✗ Invalid input. Please enter a number "
                                     "between 1 and "
                                  << defendList.size() << ": ";
                    }

                    if (destChoice == sourceChoice) {
                        std::cout << "✗ Source and destination cannot be the "
                                     "same territory."
                                  << std::endl;
                        std::cout << "✗ Advance order NOT created."
                                  << std::endl;
                        break;
                    }

                    Territory* dest = defendList[destChoice - 1];

                    std::cout << "How many armies to move: ";
                    int armies;
                    while (!(std::cin >> armies) || armies < 1) {
                        if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(
                                std::numeric_limits<std::streamsize>::max(),
                                '\n');
                        }
                        std::cout << "✗ Invalid input. Please enter a positive "
                                     "number: ";
                    }

                    // Create the order
                    issueAdvanceOrder(source, dest, armies);
                    std::cout << "✓ Advance order SUCCESSFULLY created and "
                                 "added to orders list!"
                              << std::endl;
                    std::cout << "  - Move " << armies << " armies from "
                              << source->getName() << " to " << dest->getName()
                              << std::endl;
                    std::cout
                        << "  (Order will be validated during execution phase)"
                        << std::endl;

                } else if (advanceChoice == 2) {
                    // Attack enemy territory
                    if (attackList.empty()) {
                        std::cout
                            << "✗ No enemy territories available to attack."
                            << std::endl;
                        std::cout << "✗ Advance order NOT created."
                                  << std::endl;
                        break;
                    }

                    if (defendList.empty()) {
                        std::cout << "✗ You have no territories to attack from."
                                  << std::endl;
                        std::cout << "✗ Advance order NOT created."
                                  << std::endl;
                        break;
                    }

                    std::cout << "\nYour territories:" << std::endl;
                    for (size_t i = 0; i < defendList.size(); ++i) {
                        std::cout << i + 1 << ". " << defendList[i]->getName()
                                  << " (Armies: " << defendList[i]->getArmies()
                                  << ")" << std::endl;
                    }

                    std::cout << "\nEnemy territories (to attack):"
                              << std::endl;
                    for (size_t i = 0; i < attackList.size(); ++i) {
                        std::cout << i + 1 << ". " << attackList[i]->getName()
                                  << " (Armies: " << attackList[i]->getArmies()
                                  << ")" << std::endl;
                    }

                    std::cout << "\nChoose your territory to attack from (1-"
                              << defendList.size() << "): ";
                    int sourceChoice;
                    while (!(std::cin >> sourceChoice) || sourceChoice < 1
                           || sourceChoice
                               > static_cast<int>(defendList.size())) {
                        if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(
                                std::numeric_limits<std::streamsize>::max(),
                                '\n');
                        }
                        std::cout << "✗ Invalid input. Please enter a number "
                                     "between 1 and "
                                  << defendList.size() << ": ";
                    }

                    Territory* source = defendList[sourceChoice - 1];

                    std::cout << "Choose territory to attack (1-"
                              << attackList.size() << "): ";
                    int targetChoice;
                    while (!(std::cin >> targetChoice) || targetChoice < 1
                           || targetChoice
                               > static_cast<int>(attackList.size())) {
                        if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(
                                std::numeric_limits<std::streamsize>::max(),
                                '\n');
                        }
                        std::cout << "✗ Invalid input. Please enter a number "
                                     "between 1 and "
                                  << attackList.size() << ": ";
                    }

                    Territory* target = attackList[targetChoice - 1];

                    std::cout << "How many armies to attack with: ";
                    int armies;
                    while (!(std::cin >> armies) || armies < 1) {
                        if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(
                                std::numeric_limits<std::streamsize>::max(),
                                '\n');
                        }
                        std::cout << "✗ Invalid input. Please enter a positive "
                                     "number: ";
                    }

                    // Create the order
                    issueAdvanceOrder(source, target, armies);
                    std::cout << "✓ Advance order SUCCESSFULLY created and "
                                 "added to orders list!"
                              << std::endl;
                    std::cout << "  - Attack " << target->getName() << " with "
                              << armies << " armies from " << source->getName()
                              << std::endl;
                    std::cout
                        << "  (Order will be validated during execution phase)"
                        << std::endl;
                }
                break;
            }

            case 2: { // Play a Card
                if (getCards().empty()) {
                    std::cout << "✗ You have no cards to play." << std::endl;
                    break;
                }

                if (deck == nullptr) {
                    std::cout << "✗ Error: Deck not available." << std::endl;
                    break;
                }

                std::cout << "\n[PLAY CARD]" << std::endl;
                std::cout << "Your cards:" << std::endl;
                const auto &cards = getCards();
                for (size_t i = 0; i < cards.size(); ++i) {
                    std::cout << i + 1 << ". " << cards[i]->getCardType()
                              << std::endl;
                }

                std::cout << "\nChoose card to play (1-" << cards.size()
                          << ", 0 to cancel): ";
                int cardChoice;
                while (!(std::cin >> cardChoice) || cardChoice < 0
                       || cardChoice > static_cast<int>(cards.size())) {
                    if (std::cin.fail()) {
                        std::cin.clear();
                        std::cin.ignore(
                            std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    std::cout << "✗ Invalid input. Please enter a number "
                                 "between 0 and "
                              << cards.size() << ": ";
                }

                if (cardChoice == 0) {
                    std::cout << "Card play cancelled." << std::endl;
                    break;
                }

                if (cardChoice > 0
                    && cardChoice <= static_cast<int>(cards.size())) {
                    Card* selectedCard = cards[cardChoice - 1];
                    selectedCard->play(this, deck);
                }
                break;
            }

            case 3: { // End turn
                continueIssuing = false;
                std::cout << "\n✓ Ending turn..." << std::endl;
                break;
            }
        }
    }

    std::cout << "\n=== Player " << getName()
              << " has completed their turn ===" << std::endl;
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

std::ostream &operator<<(std::ostream &os, const Player &player) {
    os << "Player: " << *player.name << "\n";
    os << "Territories owned: " << player.territories.size() << "\n";
    os << "Cards in hand: " << player.hand->getCards().size() << "\n";
    os << "Orders issued: " << player.ordersList->getOrders().size() << "\n";
    os << "Reinforcement pool: " << *player.reinforcementPool << "\n";
    return os;
}
