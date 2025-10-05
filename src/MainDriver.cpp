#include "Cards/Cards.h"
#include "Cards/CardsDriver.h"
#include "Map/Map.h"
#include "Map/MapDriver.h"
#include "Orders/Orders.h"
#include "Orders/OrdersDriver.h"
#include "Player/Player.h"
#include "Player/PlayerDriver.h"
#include "Utils/Utils.h"
#include <iostream>

int main() {
	std::cout << "Warzone Game Test Driver\n"
			  << std::endl
			  << SEPARATOR_LINE << std::endl
			  << "Map Tests\n"
			  << std::endl;

	testLoadMaps();

	std::cout << SEPARATOR_LINE << std::endl << "Cards Tests\n" << std::endl;
	testCards();

	std::cout << SEPARATOR_LINE << std::endl << "Orders Tests\n" << std::endl;
	testOrdersList();

	std::cout << SEPARATOR_LINE << std::endl << "Player Tests\n" << std::endl;
	testPlayers();

	std::cout << SEPARATOR_LINE << std::endl;

	return 0;
}