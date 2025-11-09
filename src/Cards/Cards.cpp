#include "Cards.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

//---------------------------Card-------------------------------
Card::Card(CardType cardType) : cardType(cardType) {}
Card::~Card() = default;

// Play card >> create order >> return card to deck
bool Card::play(Player* player, Deck* deck) {
    if (!player || !deck) {
        std::cout << "✗ Invalid player or deck" << std::endl;
        return false;
    }

    // Get territory lists from player (computed fresh to ensure current state)
    auto defendList = player->toDefend();
    auto attackList = player->toAttack();

    // Handle card effects
    Order* order = nullptr;

    switch (cardType) {
        case CardType::REINFORCEMENT: {
            // REINFORCEMENT card is executed immediately
            player->setReinforcementPool(player->getReinforcementPool() + 5);
            std::cout << "✓ REINFORCEMENT card played!" << std::endl;
            std::cout << "  Added 5 armies to your reinforcement pool."
                      << std::endl;
            std::cout << "  New total: " << player->getReinforcementPool()
                      << " armies" << std::endl;
            std::cout << "  (You can deploy these on your next turn or "
                         "continue with other orders)"
                      << std::endl;

            // Remove card from hand and return to deck
            player->removeCard(this);
            deck->returnCard(this);
            return true;
        }
        case CardType::BOMB: {
            if (attackList.empty()) {
                std::cout << "✗ No enemy territories available to bomb."
                          << std::endl;
                std::cout << "✗ Bomb order NOT created." << std::endl;
                return false;
            }

            std::cout << "\nChoose territory to bomb:" << std::endl;
            for (size_t i = 0; i < attackList.size(); ++i) {
                std::cout << i + 1 << ". " << attackList[i]->getName()
                          << " (Armies: " << attackList[i]->getArmies() << ")"
                          << std::endl;
            }

            int choice;
            while (!(std::cin >> choice) || choice < 1
                   || choice > static_cast<int>(attackList.size())) {
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                    '\n');
                }
                std::cout
                    << "✗ Invalid input. Please enter a number between 1 and "
                    << attackList.size() << ": ";
            }
            order = new Bomb(player, attackList[choice - 1]);
            break;
        }
        case CardType::BLOCKADE: {
            if (defendList.empty()) {
                std::cout << "✗ No territories available to blockade."
                          << std::endl;
                std::cout << "✗ Blockade order NOT created." << std::endl;
                return false;
            }

            std::cout << "\nChoose territory to blockade:" << std::endl;
            for (size_t i = 0; i < defendList.size(); ++i) {
                std::cout << i + 1 << ". " << defendList[i]->getName()
                          << " (Armies: " << defendList[i]->getArmies() << ")"
                          << std::endl;
            }

            int choice;
            while (!(std::cin >> choice) || choice < 1
                   || choice > static_cast<int>(defendList.size())) {
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                    '\n');
                }
                std::cout
                    << "✗ Invalid input. Please enter a number between 1 and "
                    << defendList.size() << ": ";
            }
            order = new Blockade(player, defendList[choice - 1]);
            break;
        }
        case CardType::AIRLIFT: {
            if (defendList.size() < 2) {
                std::cout << "✗ Need at least 2 territories for Airlift."
                          << std::endl;
                std::cout << "✗ Airlift order NOT created." << std::endl;
                return false;
            }

            std::cout << "\nChoose source territory:" << std::endl;
            for (size_t i = 0; i < defendList.size(); ++i) {
                std::cout << i + 1 << ". " << defendList[i]->getName()
                          << " (Armies: " << defendList[i]->getArmies() << ")"
                          << std::endl;
            }

            int sourceChoice;
            while (!(std::cin >> sourceChoice) || sourceChoice < 1
                   || sourceChoice > static_cast<int>(defendList.size())) {
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                    '\n');
                }
                std::cout
                    << "✗ Invalid input. Please enter a number between 1 and "
                    << defendList.size() << ": ";
            }

            Territory* source = defendList[sourceChoice - 1];

            std::cout << "\nChoose destination territory:" << std::endl;
            for (size_t i = 0; i < defendList.size(); ++i) {
                if (static_cast<int>(i) != sourceChoice - 1) {
                    std::cout << i + 1 << ". " << defendList[i]->getName()
                              << " (Armies: " << defendList[i]->getArmies()
                              << ")" << std::endl;
                }
            }

            int destChoice;
            while (!(std::cin >> destChoice) || destChoice < 1
                   || destChoice > static_cast<int>(defendList.size())) {
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                    '\n');
                }
                std::cout
                    << "✗ Invalid input. Please enter a number between 1 and "
                    << defendList.size() << ": ";
            }

            if (destChoice == sourceChoice) {
                std::cout << "✗ Source and destination cannot be the same."
                          << std::endl;
                std::cout << "✗ Airlift order NOT created." << std::endl;
                return false;
            }

            std::cout << "Enter number of armies to airlift: ";
            int armies;
            while (!(std::cin >> armies) || armies < 1) {
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                    '\n');
                }
                std::cout
                    << "✗ Invalid input. Please enter a positive number: ";
            }

            order =
                new Airlift(player, source, defendList[destChoice - 1], armies);
            break;
        }
        case CardType::DIPLOMACY: {
            std::set<Player*> enemyPlayers;
            for (Territory* t : attackList) {
                if (t->getPlayer() != player) {
                    enemyPlayers.insert(t->getPlayer());
                }
            }

            if (enemyPlayers.empty()) {
                std::cout << "✗ No enemy players available to negotiate with."
                          << std::endl;
                std::cout << "✗ Negotiate order NOT created." << std::endl;
                return false;
            }

            std::cout << "\nChoose player to negotiate with:" << std::endl;
            std::vector<Player*> players(enemyPlayers.begin(),
                                         enemyPlayers.end());
            for (size_t i = 0; i < players.size(); ++i) {
                std::cout << i + 1 << ". " << players[i]->getName()
                          << " (Controls "
                          << players[i]->getTerritories().size()
                          << " territories)" << std::endl;
            }

            int choice;
            while (!(std::cin >> choice) || choice < 1
                   || choice > static_cast<int>(players.size())) {
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                    '\n');
                }
                std::cout
                    << "✗ Invalid input. Please enter a number between 1 and "
                    << players.size() << ": ";
            }
            order = new Negotiate(player, players[choice - 1]);
            break;
        }
        case CardType::UNKNOWN: {
            std::cout << "✗ Unknown card type" << std::endl;
            return false;
        }
    }

    // If order was created, add it to player's orders list
    if (order != nullptr) {
        player->addOrder(order);
        player->removeCard(this);
        deck->returnCard(this);
        std::cout << "✓ " << cardTypeToString(cardType)
                  << " order SUCCESSFULLY created and added to orders list!"
                  << std::endl;
        std::cout << "  Card returned to deck." << std::endl;
        std::cout << "  (Order will be validated during execution phase)"
                  << std::endl;
        return true;
    } else {
        std::cout << "✗ Failed to create card order." << std::endl;
        return false;
    }
}

CardType Card::getCardType() const {
    return cardType;
}

std::ostream &operator<<(std::ostream &os, const Card &card) {
    os << cardTypeToString(card.getCardType());
    return os;
}

//---------------------------Hand-------------------------------
Hand::Hand() = default;

Hand::~Hand() {
    // Cards are owned by the deck, so we don't delete them here
    cards.clear();
}

void Hand::addCard(Card* card) {
    if (card) {
        cards.push_back(card);
    }
}

void Hand::removeCard(Card* card) {
    cards.erase(std::remove(cards.begin(), cards.end(), card), cards.end());
}

const std::vector<Card*> &Hand::getCards() const {
    return cards;
}

std::ostream &operator<<(std::ostream &os, const Hand &hand) {
    os << "Hand contains " << hand.cards.size() << " cards:" << std::endl;
    for (const Card* card : hand.cards) {
        os << "- " << *card << std::endl;
    }
    return os;
}

//---------------------------Deck-------------------------------
Deck::Deck()
    : rng(std::chrono::system_clock::now().time_since_epoch().count()) {
    initialize();
}

Deck::Deck(const Deck &other)
    : rng(std::chrono::system_clock::now().time_since_epoch().count()) {
    // Deep copy of cards
    for (Card* card : other.cards) {
        cards.push_back(new Card(card->getCardType()));
    }
}

Deck &Deck::operator=(const Deck &other) {
    if (this != &other) {
        // Clear existing cards
        for (Card* card : cards) {
            delete card;
        }
        cards.clear();

        // Deep copy of cards
        for (Card* card : other.cards) {
            cards.push_back(new Card(card->getCardType()));
        }
    }
    return *this;
}

Deck::~Deck() {
    for (Card* card : cards) {
        delete card;
    }
    cards.clear();
}

void Deck::initialize() {
    // Clear existing cards
    for (Card* card : cards) {
        delete card;
    }
    cards.clear();

    // Create cards of each type (multiple copies for game balance)
    const int numCopies = 5; // Number of copies of each card type

    for (int i = 0; i < numCopies; ++i) {
        cards.push_back(new Card(CardType::BOMB));
        cards.push_back(new Card(CardType::REINFORCEMENT));
        cards.push_back(new Card(CardType::BLOCKADE));
        cards.push_back(new Card(CardType::AIRLIFT));
        cards.push_back(new Card(CardType::DIPLOMACY));
    }

    // Shuffle the deck
    std::shuffle(cards.begin(), cards.end(), rng);
}

Card* Deck::draw() {
    if (cards.empty()) {
        return nullptr;
    }

    // Take the top card
    Card* card = cards.back();
    cards.pop_back();
    return card;
}

void Deck::returnCard(Card* card) {
    if (card) {
        cards.push_back(card);
        // Shuffle the deck after returning a card
        std::shuffle(cards.begin(), cards.end(), rng);
    }
}

size_t Deck::size() const {
    return cards.size();
}

std::ostream &operator<<(std::ostream &os, const Deck &deck) {
    os << "Deck contains " << deck.size() << " cards." << std::endl;
    return os;
}