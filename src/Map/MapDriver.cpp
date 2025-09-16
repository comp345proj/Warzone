#include "Map.h"
#include <iostream>

void runMapTests() {
	try {
		MapLoader loader;
		auto map = loader.loadMap("res/World.map");

		std::cout << "\nLoaded map successfully:\n" << *map << std::endl;

		// Test validation
		std::cout << "\nValidating map...\n";
		if (map->validate()) {
			std::cout << "Map is valid!\n";
		} else {
			std::cout << "Map is invalid!\n";
		}

		// Test territory search
		std::cout << "\nTesting territory search...\n";
		Territory* alaska = map->findTerritory("Alaska");
		if (alaska) {
			std::cout << "Found Alaska:\n  " << *alaska << "\n  Adjacent to: ";
			for (Territory* adj : alaska->getAdjacencies()) {
				std::cout << adj->getName() << ", ";
			}
			std::cout << "\n";
		}

		// Test continent bonus
		std::cout << "\nTesting continent bonus...\n";
		Continent* northAmerica = map->findContinent("North America");
		if (northAmerica) {
			std::cout << "North America bonus: " << northAmerica->getBonus()
					  << "\n";
			std::cout << "Territories in North America: ";
			for (Territory* t : northAmerica->getTerritories()) {
				std::cout << t->getName() << ", ";
			}
			std::cout << "\n";
		}

		// Test deep copy
		std::cout << "\nTesting deep copy...\n";
		Map copiedMap(*map);
		std::cout << "Original map has " << map->getTerritories().size()
				  << " territories\n";
		std::cout << "Copied map has " << copiedMap.getTerritories().size()
				  << " territories\n";

		// Verify the deep copy worked by checking adjacencies
		Territory* originalAlaska = map->findTerritory("Alaska");
		Territory* copiedAlaska = copiedMap.findTerritory("Alaska");
		if (originalAlaska && copiedAlaska) {
			std::cout << "Original Alaska has "
					  << originalAlaska->getAdjacencies().size()
					  << " adjacencies\n";
			std::cout << "Copied Alaska has "
					  << copiedAlaska->getAdjacencies().size()
					  << " adjacencies\n";
		}

	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

// Main function removed to avoid conflicts.
// Use runMapTests() to run the tests.