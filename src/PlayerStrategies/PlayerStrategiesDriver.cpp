#include "Cards/Cards.h"
#include "Map/Map.h"
#include "Player/Player.h"
#include "PlayerStrategies.h"
#include <iostream>
#include <vector>

// Free function to test player strategies
void testPlayerStrategies() {
    std::cout << "=== Testing Player Strategies ===" << std::endl;

    // Create some territories for testing
    Territory* t1 = new Territory("Territory1", 0, 0);
    Territory* t2 = new Territory("Territory2", 1, 1);
    Territory* t3 = new Territory("Territory3", 2, 2);
    Territory* t4 = new Territory("Territory4", 3, 3);

    std::cout << "Created territories: " << t1 << ", " << t2 << ", " << t3
              << ", " << t4 << std::endl;

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
    Territory* t5 = new Territory("Territory5", 4, 4);
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

    std::cout << "\n--- Demonstration 1: Different strategies lead to "
                 "different behavior ---"
              << std::endl;

    // Test toDefend() - different prioritization
    std::cout << "Human player defends: ";
    auto humanDefend = humanPlayer->toDefend();
    for (auto t : humanDefend)
        std::cout << t->getName() << "(" << t->getArmies() << ") ";
    std::cout << std::endl;

    std::cout << "Aggressive player defends: ";
    auto aggressiveDefend = aggressivePlayer->toDefend();
    for (auto t : aggressiveDefend)
        std::cout << t->getName() << "(" << t->getArmies() << ") ";
    std::cout << std::endl;

    std::cout << "Benevolent player defends: ";
    auto benevolentDefend = benevolentPlayer->toDefend();
    for (auto t : benevolentDefend)
        std::cout << t->getName() << "(" << t->getArmies() << ") ";
    std::cout << std::endl;

    // Test toAttack() - different targeting
    std::cout << "Aggressive player attacks: ";
    auto aggressiveAttack = aggressivePlayer->toAttack();
    for (auto t : aggressiveAttack)
        std::cout << t->getName() << "(" << t->getArmies() << ") ";
    std::cout << std::endl;

    std::cout << "Benevolent player attacks: ";
    auto benevolentAttack = benevolentPlayer->toAttack();
    std::cout << (benevolentAttack.empty() ? "None (benevolent never attacks)"
                                           : "Some targets")
              << std::endl;

    std::cout << "Neutral player attacks: ";
    auto neutralAttack = neutralPlayer->toAttack();
    std::cout << (neutralAttack.empty() ? "None (neutral never attacks)"
                                        : "Some targets")
              << std::endl;

    std::cout << "\n--- Demonstration 2: Dynamic strategy changes ---"
              << std::endl;

    std::cout << "Neutral player strategy: NeutralPlayerStrategy" << std::endl;
    std::cout << "Neutral player attacks before change: ";
    auto oldNeutralAttack = neutralPlayer->toAttack();
    std::cout << (oldNeutralAttack.empty() ? "None (neutral never attacks)"
                                           : "Some targets")
              << std::endl;

    // Simulate an attack on neutral player (this would normally happen in
    // Advance::execute)
    neutralPlayer->setStrategy(new AggressivePlayerStrategy());
    std::cout << "After being attacked, neutral player strategy changed to: "
                 "AggressivePlayerStrategy"
              << std::endl;

    std::cout << "New attack targets: ";
    auto newNeutralAttack = neutralPlayer->toAttack();
    for (auto t : newNeutralAttack)
        std::cout << t->getName() << "(" << t->getArmies() << ") ";
    std::cout << std::endl;

    std::cout << "\n--- Demonstration 3: Human vs Computer behavior ---"
              << std::endl;

    std::cout << "Human player issueOrder() requires user interaction "
                 "(interactive mode)"
              << std::endl;
    std::cout << "Computer players issueOrder() automatically:" << std::endl;

    std::cout << "Aggressive player issuing orders:" << std::endl;
    aggressivePlayer->issueOrder(deck);

    std::cout << "Benevolent player issuing orders:" << std::endl;
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