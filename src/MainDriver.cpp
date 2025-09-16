#include "Map/Map.h"
#include "Player/Player.h"
#include <iostream>

// Function declarations from other drivers
void runMapTests();
void testPlayers();

int main() {
	std::cout << "Warzone Game Test Driver\n" << std::endl;

	std::cout << "\n=== Testing Map Component ===\n";
	runMapTests();

	std::cout << "\n=== Testing Player Component ===\n";
	testPlayers();

	return 0;
}