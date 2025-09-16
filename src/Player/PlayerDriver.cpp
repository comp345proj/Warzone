#include "Player.h"
#include <iostream>

void testPlayers() {
	std::cout << "Testing Player Functionality..." << std::endl;

	try {
		// Create a player
		Player player("TestPlayer");
		std::cout << "Created player: " << player << std::endl;

		// Test toDefend()
		auto defenseTerritories = player.toDefend();
		std::cout << "Territories to defend: " << defenseTerritories.size()
				  << std::endl;

		// Test toAttack()
		auto attackTerritories = player.toAttack();
		std::cout << "Territories to attack: " << attackTerritories.size()
				  << std::endl;

		// Test issueOrder()
		player.issueOrder();
		std::cout << "Order issued successfully" << std::endl;

	} catch (const std::exception &e) {
		std::cout << "Error during player testing: " << e.what() << std::endl;
	}
}
