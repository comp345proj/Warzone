#include "Cards/Cards.h"
#include "Map/Map.h"
#include "Player/Player.h"
#include "PlayerStrategies.h"
#include <iostream>
#include <vector>

// Free function to test player strategies
void testPlayerStrategies() {
    std::cout << "=== Testing Player Strategies ===\n" << std::endl;

    // Create some territories for testing
    Territory* t1 = new Territory("T1", 0, 0);
    Territory* t2 = new Territory("T2", 1, 1);
    Territory* t3 = new Territory("T3", 2, 2);
    Territory* t4 = new Territory("T4", 3, 3);

    // Make territories adjacent
    // t1 <-> t2 <-> t3 <-> t4
    t1->addAdjacentTerritory(t2);
    t2->addAdjacentTerritory(t1);

    t2->addAdjacentTerritory(t3);
    t3->addAdjacentTerritory(t2);

    t3->addAdjacentTerritory(t4);
    t4->addAdjacentTerritory(t3);

    // Create players with different strategies
    Player* humanPlayer = new Player("Human player", new HumanPlayerStrategy());
    Player* aggressivePlayer =
        new Player("Aggressive player", new AggressivePlayerStrategy());
    Player* benevolentPlayer =
        new Player("Benevolent player", new BenevolentPlayerStrategy());
    Player* neutralPlayer =
        new Player("Neutral player", new NeutralPlayerStrategy());
    Player* cheaterPlayer =
        new Player("Cheater player", new CheaterPlayerStrategy());

    // Assign territories
    humanPlayer->addTerritory(t1);
    t1->setPlayer(humanPlayer);
    t1->setArmies(5);

    aggressivePlayer->addTerritory(t2);
    t2->setPlayer(aggressivePlayer);
    t2->setArmies(8);

    benevolentPlayer->addTerritory(t3);
    t3->setPlayer(benevolentPlayer);
    t3->setArmies(3);

    neutralPlayer->addTerritory(t4);
    t4->setPlayer(neutralPlayer);
    t4->setArmies(2);

    // Give cheater player a territory adjacent to others
    Territory* t5 = new Territory("T5", 4, 4);
    t4->addAdjacentTerritory(t5);
    t5->addAdjacentTerritory(t4);
    cheaterPlayer->addTerritory(t5);
    t5->setPlayer(cheaterPlayer);
    t5->setArmies(1);

    // Give some cards
    humanPlayer->addCard(new Card(CardType::BOMB));
    aggressivePlayer->addCard(new Card(CardType::AIRLIFT));
    benevolentPlayer->addCard(new Card(CardType::REINFORCEMENT));

    // Set reinforcement pools
    humanPlayer->setReinforcementPool(3);
    humanPlayer->resetAvailableReinforcementPool();

    aggressivePlayer->setReinforcementPool(4);
    aggressivePlayer->resetAvailableReinforcementPool();

    benevolentPlayer->setReinforcementPool(2);
    benevolentPlayer->resetAvailableReinforcementPool();

    // Create a deck
    Deck* deck = new Deck();
    deck->initialize();

    std::cout << "Created territories:\n"
              << *t1 << "\n"
              << *t2 << "\n"
              << *t3 << "\n"
              << *t4 << std::endl;

    std::cout << "\n--- Demonstration 1: Different strategies lead to "
                 "different behavior ---\n"
              << std::endl;

    // Test toDefend() - different prioritization
    std::cout << "Human player defends:\n";
    for (auto t : humanPlayer->toDefend())
        std::cout << *t << "\n";
    std::cout << std::endl;

    std::cout << "Human player attacks:\n";
    for (auto t : humanPlayer->toAttack())
        std::cout << *t << "\n";
    std::cout << std::endl;

    std::cout << "\nAggressive player defends:\n";
    for (auto t : aggressivePlayer->toDefend())
        std::cout << *t << "\n";
    std::cout << std::endl;

    std::cout << "\nAggressive player attacks:\n";
    for (auto t : aggressivePlayer->toAttack())
        std::cout << *t << "\n";
    std::cout << std::endl;

    std::cout << "\nBenevolent player defends:\n";
    for (auto t : benevolentPlayer->toDefend())
        std::cout << *t << "\n";
    std::cout << std::endl;

    std::cout << "\nBenevolent player attacks:\n";
    for (auto t : benevolentPlayer->toAttack())
        std::cout << *t << "\n";
    std::cout << std::endl;

    std::cout << "\nNeutral player defends:\n";
    for (auto t : neutralPlayer->toDefend())
        std::cout << *t << "\n";
    std::cout << std::endl;

    std::cout << "\nNeutral player attacks:\n";
    for (auto t : neutralPlayer->toAttack())
        std::cout << *t << "\n";
    std::cout << std::endl;

    std::cout << "\n--- Demonstration 2: Dynamic strategy changes ---\n"
              << std::endl;

    PlayerStrategy* currentStrategy = neutralPlayer->getStrategy();
    std::cout << "Neutral player strategy before change: "
              << typeid(*currentStrategy).name() << std::endl;

    std::cout << "\nNeutral player attacks:\n";
    for (auto t : neutralPlayer->toAttack())
        std::cout << *t << "\n";
    std::cout << std::endl;

    // Simulate an attack on neutral player
    neutralPlayer->setStrategy(new AggressivePlayerStrategy());
    currentStrategy = neutralPlayer->getStrategy();
    std::cout << "After being attacked, neutral player strategy changed to: "
              << typeid(*currentStrategy).name() << std::endl;

    std::cout << "\nNeutral player attacks:\n";
    for (auto t : neutralPlayer->toAttack())
        std::cout << *t << "\n";
    std::cout << std::endl;

    std::cout << "\n--- Demonstration 3: Human vs Computer behavior ---\n"
              << std::endl;

    std::cout << "Human player issueOrder()" << std::endl;
    std::cout << "Calling Human player issueOrder():\n" << std::endl;
    humanPlayer->issueOrder(deck);

    std::cout << "\nComputer players issueOrder() automatically:" << std::endl;

    aggressivePlayer->issueOrder(deck);

    benevolentPlayer->issueOrder(deck);

    std::cout << "Neutral player issuing orders:" << std::endl;
    neutralPlayer->issueOrder(deck);

    std::cout << "Cheater player issuing orders:" << std::endl;
    cheaterPlayer->issueOrder(deck);

    // Clean up
    delete humanPlayer;
    delete aggressivePlayer;
    delete benevolentPlayer;
    delete neutralPlayer;
    delete cheaterPlayer;
    delete t1;
    delete t2;
    delete t3;
    delete t4;
    delete t5;
    delete deck;

    std::cout << "\n=== Test completed ===" << std::endl;
}