#include "OrdersDriver.h"
#include "Orders.h"
#include <iostream>

void testOrdersList() {
	std::cout << "Testing Orders and OrdersList functionality" << std::endl;
	std::cout << "==========================================" << std::endl;

	// Create an orders list
	OrdersList ordersList;

	// Create orders of each type
	Order* deploy = new Deploy();
	Order* advance = new Advance();
	Order* bomb = new Bomb();
	Order* blockade = new Blockade();
	Order* airlift = new Airlift();
	Order* negotiate = new Negotiate();

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

	std::cout << "\nTest completed successfully!" << std::endl;
}