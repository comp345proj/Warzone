#include "Player.h"
#include "Map/Map.h"
#include "Cards/Cards.h"
#include "Orders/Orders.h"
#include <iostream>

void testPlayers() {
    std::cout << "Testing Player Functionality" << std::endl;
    std::cout << "=========================" << std::endl;

    try {
        // Create a player
        std::cout << "\nCreating player:" << std::endl;
        Player player("TestPlayer");
        std::cout << player << std::endl;

        // Test territory management
        std::cout << "\nTesting territory management:" << std::endl;
        Territory* territory1 = new Territory("Territory1", 0, 0);
        Territory* territory2 = new Territory("Territory2", 1, 1);
        
        player.addTerritory(territory1);
        player.addTerritory(territory2);
        std::cout << "Added territories. Player now has " 
                  << player.getTerritories().size() << " territories." << std::endl;

        // Test card management
        std::cout << "\nTesting card management:" << std::endl;
        Deck deck;  // Create a deck
        Card* card1 = deck.draw();
        Card* card2 = deck.draw();
        if (card1 && card2) {
            player.addCard(card1);
            player.addCard(card2);
            std::cout << "Added cards to player's hand. Player now has "
                      << player.getCards().size() << " cards." << std::endl;
        }

        // Test toDefend()
        std::cout << "\nTesting toDefend():" << std::endl;
        auto defenseTerritories = player.toDefend();
        std::cout << "Territories to defend: " << defenseTerritories.size() << std::endl;

        // Test toAttack()
        std::cout << "\nTesting toAttack():" << std::endl;
        auto attackTerritories = player.toAttack();
        std::cout << "Territories to attack: " << attackTerritories.size() << std::endl;

        // Test issueOrder(), which is playing a card
        std::cout << "\nTesting issueOrder():" << std::endl;

		// Pick the last card in hand to play
        player.issueOrder(player.getCards().back(), &deck);
        std::cout << "Orders in list: " << player.getOrdersList()->getOrders().size() << std::endl;

        // Clean up
        delete territory1;
        delete territory2;

        std::cout << "\nFinal player state:" << std::endl;
        std::cout << player << std::endl;

        std::cout << "\nTest completed successfully!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error during player testing: " << e.what() << std::endl;
    }
}
