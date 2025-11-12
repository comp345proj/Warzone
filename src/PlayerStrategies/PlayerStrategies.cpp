#include "PlayerStrategies.h"
#include "Cards/Cards.h"
#include "Orders/Orders.h"
#include <algorithm>
#include <iostream>
#include <limits>

// Helper function to check if two territories are adjacent
bool areAdjacent(Territory* t1, Territory* t2) {
    const auto &neighbors = t1->getAdjacentTerritories();
    return std::find(neighbors.begin(), neighbors.end(), t2) != neighbors.end();
}

// Helper function to get enemy territories adjacent to player's territories
std::vector<Territory*> getAdjacentEnemyTerritories(Player* player) {
    std::vector<Territory*> enemyTerritories;
    std::set<Territory*> uniqueEnemies;

    for (Territory* territory : player->getTerritories()) {
        for (Territory* neighbor : territory->getAdjacentTerritories()) {
            if (neighbor->getPlayer() != player) {
                uniqueEnemies.insert(neighbor);
            }
        }
    }

    enemyTerritories.assign(uniqueEnemies.begin(), uniqueEnemies.end());
    return enemyTerritories;
}

//---------------------------HumanStrategy-------------------------------
void HumanPlayerStrategy::issueOrder(Player* player, Deck* deck) {
    std::cout << "\n=== Player " << player->getName()
              << "'s Issue Order Phase ===" << std::endl;

    // PHASE 1: FORCED DEPLOYMENT - Must empty reinforcement pool first
    while (player->getAvailableReinforcementPool() > 0) {
        std::cout << "\n[DEPLOY PHASE - MANDATORY]" << std::endl;
        std::cout << "You have " << player->getAvailableReinforcementPool()
                  << " armies in your reinforcement pool that MUST be deployed."
                  << std::endl;

        auto territoriesToDefend = toDefend(player);
        if (territoriesToDefend.empty()) {
            std::cout << "ERROR: No territories to deploy to!" << std::endl;
            break;
        }

        std::cout << "\nAvailable territories to deploy armies:" << std::endl;
        for (size_t i = 0; i < territoriesToDefend.size(); ++i) {
            std::cout << i + 1 << ". " << *territoriesToDefend[i] << std::endl;
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
                  << player->getAvailableReinforcementPool() << "): ";
        int armies;
        while (!(std::cin >> armies) || armies < 1
               || armies > player->getAvailableReinforcementPool()) {
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
            }
            std::cout << "Invalid input. Please enter a number between 1 and "
                      << player->getAvailableReinforcementPool() << ": ";
        }

        Deploy* deployOrder = new Deploy(player, target, armies);
        player->addOrder(deployOrder);
        // Deduct from available reinforcement pool (temporary working pool for
        // UI only)
        player->decrementAvailableReinforcementPool(armies);
        std::cout << "✓ Deploy order created: " << armies << " armies to "
                  << target->getName() << std::endl;

        if (player->getAvailableReinforcementPool() > 0) {
            std::cout << "\nYou still have "
                      << player->getAvailableReinforcementPool()
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
        auto defendList = toDefend(player);
        auto attackList = toAttack(player);

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
                        std::cout << i + 1 << ". " << *defendList[i]
                                  << std::endl;
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
                    player->issueAdvanceOrder(source, dest, armies);
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
                        std::cout << i + 1 << ". " << *defendList[i]
                                  << std::endl;
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

                    std::cout << "\nEnemy territories (to attack):"
                              << std::endl;
                    for (size_t i = 0; i < attackList.size(); ++i) {
                        std::cout << i + 1 << ". " << *attackList[i]
                                  << std::endl;
                    }

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
                    player->issueAdvanceOrder(source, target, armies);
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
                if (player->getCards().empty()) {
                    std::cout << "✗ You have no cards to play." << std::endl;
                    break;
                }

                if (deck == nullptr) {
                    std::cout << "✗ Error: Deck not available." << std::endl;
                    break;
                }

                std::cout << "\n[PLAY CARD]" << std::endl;
                std::cout << "Your cards:" << std::endl;
                const auto &cards = player->getCards();
                for (size_t i = 0; i < cards.size(); ++i) {
                    std::cout << i + 1 << ". " << *cards[i] << std::endl;
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
                    selectedCard->play(player, deck);
                }
                break;
            }

            case 3: { // End turn
                continueIssuing = false;
                std::cout << "\nEnding turn..." << std::endl;
                break;
            }
        }
    }

    std::cout << "\n=== Player " << player->getName()
              << " has completed their turn ===" << std::endl;
}

std::vector<Territory*> HumanPlayerStrategy::toDefend(Player* player) {
    std::vector<Territory*> toDefendList = player->getTerritories();

    // Sort territories by priority (in this case, by number of armies)
    std::sort(
        toDefendList.begin(), toDefendList.end(),
        [](Territory* a, Territory* b) {
            return a->getArmies()
                < b->getArmies(); // Prioritize territories with fewer armies
        });

    return toDefendList;
}

std::vector<Territory*> HumanPlayerStrategy::toAttack(Player* player) {
    return getAdjacentEnemyTerritories(player);
}

//---------------------------AggressiveStrategy--------------------------
void AggressivePlayerStrategy::issueOrder(Player* player, Deck* deck) {
    std::cout << "\n[Aggressive Player " << player->getName()
              << " issuing orders]" << std::endl;

    // Get territory lists once
    auto defendList = toDefend(player);
    auto attackList = toAttack(player);

    // Deploy all reinforcements to strongest territory
    int deployedArmies = 0; // Track how many we deploy
    if (player->getAvailableReinforcementPool() > 0) {
        if (!defendList.empty()) {
            Territory* strongest =
                defendList.back(); // Last in sorted list (most armies)
            int armiesToDeploy = player->getAvailableReinforcementPool();
            deployedArmies = armiesToDeploy; // Remember this amount
            Deploy* deployOrder = new Deploy(player, strongest, armiesToDeploy);
            player->addOrder(deployOrder);
            player->decrementAvailableReinforcementPool(armiesToDeploy);
            std::cout << "✓ Deployed " << armiesToDeploy << " armies to "
                      << strongest->getName() << std::endl;
        }
    }

    // Play aggressive cards (Bomb, Airlift)
    for (Card* card : player->getCards()) {
        CardType type = card->getCardType();
        Order* cardOrder = nullptr;

        if (type == CardType::BOMB && !attackList.empty()) {
            // Bomb the weakest enemy territory
            cardOrder = new Bomb(player, attackList.front());
            std::cout << "✓ Playing BOMB card on "
                      << attackList.front()->getName() << std::endl;
        } else if (type == CardType::AIRLIFT && defendList.size() >= 2
                   && !attackList.empty()) {
            // Airlift armies to strongest territory adjacent to enemies
            Territory* source = defendList.front(); // Weakest
            Territory* dest = defendList.back();    // Strongest
            int armies = source->getArmies() / 2;   // Move half
            if (armies > 0) {
                cardOrder = new Airlift(player, source, dest, armies);
                std::cout << "✓ Playing AIRLIFT card: " << armies
                          << " armies from " << source->getName() << " to "
                          << dest->getName() << std::endl;
            }
        }

        if (cardOrder) {
            player->addOrder(cardOrder);
            player->removeCard(card);
            deck->returnCard(card);
            break; // Play only one card per turn
        }
    }

    // Issue advance orders to attack
    // Aggressive player always advances to enemy territories
    // Use all armies from strongest territory (after deployment)
    if (!attackList.empty() && !defendList.empty()) {
        Territory* source =
            defendList.back(); // Strongest territory (where we deployed)
        Territory* target = attackList.front(); // Weakest enemy

        // Use the armies that will be on the territory after deployment executes
        int currentArmies = source->getArmies();
        int totalArmies = currentArmies + deployedArmies; // Use the tracked deployment
        int armies = totalArmies > 1 ? totalArmies - 1
                                     : totalArmies; // Leave at least 1

        if (armies > 0) {
            player->issueAdvanceOrder(source, target, armies);
            std::cout << "✓ Issued advance order to attack "
                      << target->getName() << " with " << armies << " armies"
                      << std::endl;
        }
    }
}

std::vector<Territory*> AggressivePlayerStrategy::toDefend(Player* player) {
    std::vector<Territory*> toDefendList = player->getTerritories();

    // Sort by number of armies (strongest first)
    std::sort(toDefendList.begin(), toDefendList.end(),
              [](Territory* a, Territory* b) {
                  return a->getArmies() > b->getArmies();
              });

    return toDefendList;
}

std::vector<Territory*> AggressivePlayerStrategy::toAttack(Player* player) {
    auto enemyTerritories = getAdjacentEnemyTerritories(player);

    // Sort by number of armies (weakest first)
    std::sort(enemyTerritories.begin(), enemyTerritories.end(),
              [](Territory* a, Territory* b) {
                  return a->getArmies() < b->getArmies();
              });

    return enemyTerritories;
}

//---------------------------BenevolentStrategy--------------------------
void BenevolentPlayerStrategy::issueOrder(Player* player, Deck* deck) {
    std::cout << "\n[Benevolent Player " << player->getName()
              << " issuing orders]" << std::endl;

    // Deploy all reinforcements to weakest territory
    if (player->getAvailableReinforcementPool() > 0) {
        auto defendList = toDefend(player);
        if (!defendList.empty()) {
            Territory* weakest =
                defendList.front(); // First in sorted list (fewest armies)
            int armiesToDeploy = player->getAvailableReinforcementPool();
            Deploy* deployOrder = new Deploy(player, weakest, armiesToDeploy);
            player->addOrder(deployOrder);
            player->decrementAvailableReinforcementPool(armiesToDeploy);
            std::cout << "✓ Deployed " << armiesToDeploy << " armies to "
                      << weakest->getName() << std::endl;
        }
    }

    // Play non-harmful cards (Reinforcement, Diplomacy) - make decisions
    // automatically
    auto attackList = toAttack(player);

    for (Card* card : player->getCards()) {
        CardType type = card->getCardType();
        Order* cardOrder = nullptr;

        if (type == CardType::REINFORCEMENT) {
            // Reinforcement card is executed immediately (adds to pool)
            player->setReinforcementPool(player->getReinforcementPool() + 5);
            player->removeCard(card);
            deck->returnCard(card);
            std::cout << "✓ Played REINFORCEMENT card: +5 armies to pool (now "
                      << player->getReinforcementPool() << ")" << std::endl;
            break;
        } else if (type == CardType::DIPLOMACY && !attackList.empty()) {
            // Negotiate with the strongest enemy player
            std::set<Player*> enemyPlayers;
            for (Territory* t : attackList) {
                if (t->getPlayer() != player) {
                    enemyPlayers.insert(t->getPlayer());
                }
            }
            if (!enemyPlayers.empty()) {
                // Pick the player with the most territories (strongest threat)
                Player* targetPlayer =
                    *std::max_element(enemyPlayers.begin(), enemyPlayers.end(),
                                      [](Player* a, Player* b) {
                                          return a->getTerritories().size()
                                              < b->getTerritories().size();
                                      });
                cardOrder = new Negotiate(player, targetPlayer);
                std::cout << "✓ Playing DIPLOMACY card with "
                          << targetPlayer->getName() << std::endl;
            }
        }

        if (cardOrder) {
            player->addOrder(cardOrder);
            player->removeCard(card);
            deck->returnCard(card);
            break; // Play only one card per turn
        }
    }

    // Move armies to weakest territories (no attacks)
    auto defendList = toDefend(player);
    if (defendList.size() >= 2) {
        Territory* source = defendList.back(); // Strongest
        Territory* dest = defendList.front();  // Weakest
        int armies =
            (source->getArmies() - 1) / 2; // Move half, leave some behind
        if (armies > 0 && areAdjacent(source, dest)) {
            player->issueAdvanceOrder(source, dest, armies);
            std::cout << "✓ Moved " << armies << " armies from "
                      << source->getName() << " to " << dest->getName()
                      << std::endl;
        }
    }
}

std::vector<Territory*> BenevolentPlayerStrategy::toDefend(Player* player) {
    std::vector<Territory*> toDefendList = player->getTerritories();

    // Sort by number of armies (weakest first)
    std::sort(toDefendList.begin(), toDefendList.end(),
              [](Territory* a, Territory* b) {
                  return a->getArmies() < b->getArmies();
              });

    return toDefendList;
}

std::vector<Territory*>
BenevolentPlayerStrategy::toAttack([[maybe_unused]] Player* player) {
    // Benevolent players never attack
    return std::vector<Territory*>();
}

//---------------------------NeutralStrategy-----------------------------
void NeutralPlayerStrategy::issueOrder(Player* player,
                                       [[maybe_unused]] Deck* deck) {
    std::cout << "\n[Neutral Player " << player->getName()
              << " issuing orders - does nothing]" << std::endl;
    // Neutral players issue no orders and play no cards
}

std::vector<Territory*> NeutralPlayerStrategy::toDefend(Player* player) {
    return player->getTerritories();
}

std::vector<Territory*>
NeutralPlayerStrategy::toAttack([[maybe_unused]] Player* player) {
    return std::vector<Territory*>();
}

//---------------------------CheaterStrategy-----------------------------
void CheaterPlayerStrategy::issueOrder(Player* player,
                                       [[maybe_unused]] Deck* deck) {
    std::cout << "\n[Cheater Player " << player->getName() << " issuing orders]"
              << std::endl;

    // Conquer all adjacent enemy territories once per turn
    if (!player->getHasCheatedThisTurn()) {
        auto attackList = toAttack(player);
        for (Territory* target : attackList) {
            Player* previousOwner = target->getPlayer();
            if (previousOwner) {
                previousOwner->removeTerritory(target);
            }
            target->setPlayer(player);
            target->setArmies(1); // Set to 1 army
            player->addTerritory(target);
            std::cout << "✓ Cheated: Conquered " << target->getName()
                      << std::endl;
        }
        player->setHasCheatedThisTurn(true);
    } else {
        std::cout << "✓ Already cheated this turn" << std::endl;
    }

    // Cheater doesn't play cards
}

std::vector<Territory*> CheaterPlayerStrategy::toDefend(Player* player) {
    return player->getTerritories();
}

std::vector<Territory*> CheaterPlayerStrategy::toAttack(Player* player) {
    return getAdjacentEnemyTerritories(player);
}