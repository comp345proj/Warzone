#include "OrdersDriver.h"
#include "Map/Map.h"
#include "Orders.h"
#include "Player/Player.h"
#include <iostream>

// Main function to test order execution implementation
void testOrderExecution() {
    std::cout << "\n=== Testing Order Execution ===\n" << std::endl;

    // Create a simple test map
    Map* map = new Map(false, false, "Test", "", "Test Map", "");

    // Create territories
    Territory* t1 = new Territory("Territory 1", 0, 0);
    Territory* t2 = new Territory("Territory 2", 1, 0);
    Territory* t3 = new Territory("Territory 3", 0, 1);
    Territory* t4 = new Territory("Territory 4", 1, 1);

    // Set up adjacencies
    t1->addAdjacentTerritory(t2);
    t2->addAdjacentTerritory(t1);
    t1->addAdjacentTerritory(t3);
    t3->addAdjacentTerritory(t1);
    t2->addAdjacentTerritory(t4);
    t4->addAdjacentTerritory(t2);
    t3->addAdjacentTerritory(t4);
    t4->addAdjacentTerritory(t3);

    // Add territories to map
    map->addTerritory(t1);
    map->addTerritory(t2);
    map->addTerritory(t3);
    map->addTerritory(t4);

    // Create players
    Player* p1 = new Player("Player 1");
    Player* p2 = new Player("Player 2");

    // Assign territories
    t1->setPlayer(p1);
    t2->setPlayer(p1);
    t3->setPlayer(p2);
    t4->setPlayer(p2);

    p1->addTerritory(t1);
    p1->addTerritory(t2);
    p2->addTerritory(t3);
    p2->addTerritory(t4);

    // Set initial armies
    t1->setArmies(10);
    t2->setArmies(5);
    t3->setArmies(8);
    t4->setArmies(6);

    std::cout << "\n1. Testing Deploy Order Validation" << std::endl;
    Deploy* validDeploy = new Deploy(p1, t1, 5);
    Deploy* invalidDeploy = new Deploy(p1, t3, 5); // Invalid: enemy territory

    std::cout << "Valid deploy order validation: "
              << (validDeploy->validate() ? "passed" : "failed") << std::endl;
    std::cout << "Invalid deploy order validation: "
              << (!invalidDeploy->validate() ? "passed" : "failed")
              << std::endl;

    validDeploy->execute();
    invalidDeploy->execute();

    std::cout << "\n2. Testing Advance Order and Territory Conquest"
              << std::endl;
    Advance* attack = new Advance(p1, t1, t3, 8);
    std::cout << "Initial armies in T1: " << t1->getArmies() << std::endl;
    std::cout << "Initial armies in T3: " << t3->getArmies() << std::endl;

    attack->execute();

    std::cout << "After attack:" << std::endl;
    std::cout << "T3 owner: "
              << (t3->getPlayer() == p1 ? "Player 1 (Conquered)" : "Player 2")
              << std::endl;
    std::cout << "Remaining armies in T3: " << t3->getArmies() << std::endl;

    std::cout << "\n3. Testing Card Award for Conquest" << std::endl;
    int initialCards = p1->getCards().size();
    // The card should have been awarded in the previous advance execution if
    // conquest was successful
    std::cout << "Cards before conquest: " << initialCards << std::endl;
    std::cout << "Cards after conquest: " << p1->getCards().size() << std::endl;

    std::cout << "\n4. Testing Negotiate Order" << std::endl;
    Negotiate* negotiate = new Negotiate(p1, p2);
    negotiate->execute();

    // Try to attack after negotiation
    Advance* blockedAttack = new Advance(p1, t1, t4, 5);
    blockedAttack->execute();
    std::cout << "Attack after negotiation should be blocked" << std::endl;

    std::cout << "\n5. Testing Blockade Order" << std::endl;
    t2->setArmies(4);
    Blockade* blockade = new Blockade(p1, t2);
    std::cout << "Initial armies in T2: " << t2->getArmies() << std::endl;
    std::cout << "Initial owner of T2: "
              << (t2->getPlayer() == p1 ? "Player 1" : "Other") << std::endl;

    blockade->execute();

    std::cout << "After blockade:" << std::endl;
    std::cout << "Armies in T2: " << t2->getArmies() << std::endl;
    std::cout << "Owner of T2: "
              << (t2->getPlayer() == Blockade::getNeutralPlayer()
                      ? "Neutral"
                      : "Not Neutral")
              << std::endl;

    std::cout << "\n6. Testing All Order Types" << std::endl;

    // Test Bomb
    Bomb* bomb = new Bomb(p2, t1);
    std::cout << "Testing Bomb order..." << std::endl;
    bomb->execute();

    // Test Airlift
    Airlift* airlift = new Airlift(p2, t3, t4, 3);
    std::cout << "Testing Airlift order..." << std::endl;
    airlift->execute();

    // Cleanup
    delete map;
    delete p1;
    delete p2;
    delete validDeploy;
    delete invalidDeploy;
    delete attack;
    delete blockedAttack;
    delete negotiate;
    delete blockade;
    delete bomb;
    delete airlift;

    std::cout << "\n=== Order Execution Testing Complete ===" << std::endl;
}

void testOrdersList() {
    std::cout << "\n=== Testing Orders and OrdersList ===\n" << std::endl;

    // Create an orders list
    OrdersList ordersList;

    // Create dummy objects for testing
    Player* dummyPlayer = new Player("DummyPlayer");
    Territory* dummyTerritory1 = new Territory("DummyTerritory1", 0, 0);
    Territory* dummyTerritory2 = new Territory("DummyTerritory2", 1, 0);
    dummyTerritory1->setPlayer(dummyPlayer);
    dummyPlayer->addTerritory(dummyTerritory1);

    // Create orders of each type with dummy parameters
    Order* deploy = new Deploy(dummyPlayer, dummyTerritory1, 5);
    Order* advance =
        new Advance(dummyPlayer, dummyTerritory1, dummyTerritory2, 3);
    Order* bomb = new Bomb(dummyPlayer, dummyTerritory2);
    Order* blockade = new Blockade(dummyPlayer, dummyTerritory1);
    Order* airlift =
        new Airlift(dummyPlayer, dummyTerritory1, dummyTerritory2, 2);
    Order* negotiate = new Negotiate(dummyPlayer, new Player("DummyPlayer2"));

    // Add orders to the list
    std::cout << "\nAdding orders to the list:" << std::endl;
    ordersList.addOrder(deploy);
    ordersList.addOrder(advance);
    ordersList.addOrder(bomb);
    ordersList.addOrder(blockade);
    ordersList.addOrder(airlift);
    ordersList.addOrder(negotiate);

    // Print initial list
    std::cout << "\nInitial orders list:" << std::endl;
    std::cout << ordersList;

    // Test moving an order
    std::cout << "\nMoving 'bomb' order to position 1:" << std::endl;
    ordersList.move(bomb, 1);
    std::cout << ordersList;

    // Test removing an order
    std::cout << "\nRemoving 'blockade' order:" << std::endl;
    ordersList.remove(blockade);
    std::cout << ordersList;

    // Test executing orders
    std::cout << "\nExecuting remaining orders:" << std::endl;
    for (const Order* order : ordersList.getOrders()) {
        const_cast<Order*>(order)->execute();
    }

    // Print final list showing effects
    std::cout << "\nFinal orders list with effects:" << std::endl;
    std::cout << ordersList;

    // Cleanup dummy objects
    delete dummyTerritory1;
    delete dummyTerritory2;
    delete dummyPlayer;
    // Note: Orders are deleted by the OrdersList destructor

    std::cout << "\nTest completed successfully!" << std::endl;
}