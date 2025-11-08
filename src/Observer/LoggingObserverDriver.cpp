// Driver to demonstrate the logging observer behaviour
#include "Observer/LoggingObserverDriver.h"
#include "Cards/Cards.h"
#include "CommandProcessor/CommandProcessor.h"
#include "GameEngine/GameEngine.h"
#include "Observer/LogObserver.h"
#include "Orders/Orders.h"
#include "Player/Player.h"
#include <cstdio>
#include <fstream>
#include <iostream>

// Free function required by the assignment
void testLoggingObserver() {
	// Clear previous log
	std::remove("gamelog.txt");

	LogObserver* logger = new LogObserver();

	std::cout << "-- Verifying class relationships --" << std::endl;
	CommandProcessor cp;
	Command cmd("sample");
	// Create a test player and territory for deploy order
	Player* testPlayer = new Player("TestPlayer");
	Territory* testTerritory = new Territory("TestTerritory", 0, 0);
	Deploy* deployOrder = new Deploy(testPlayer, testTerritory, 5);
	OrdersList ordersList;
	GameEngine ge;

	// Check that these objects are ILoggable and Subject via dynamic_cast
	std::cout << "CommandProcessor is ILoggable: "
			  << (dynamic_cast<ILoggable*>(&cp) != nullptr) << std::endl;
	std::cout << "Command is ILoggable: "
			  << (dynamic_cast<ILoggable*>(&cmd) != nullptr) << std::endl;
	std::cout << "Order (Deploy) is ILoggable: "
			  << (dynamic_cast<ILoggable*>(deployOrder) != nullptr)
			  << std::endl;
	std::cout << "OrdersList is ILoggable: "
			  << (dynamic_cast<ILoggable*>(&ordersList) != nullptr)
			  << std::endl;
	std::cout << "GameEngine is ILoggable: "
			  << (dynamic_cast<ILoggable*>(&ge) != nullptr) << std::endl;

	std::cout << "CommandProcessor is Subject: "
			  << (dynamic_cast<Subject*>(&cp) != nullptr) << std::endl;
	std::cout << "Command is Subject: "
			  << (dynamic_cast<Subject*>(&cmd) != nullptr) << std::endl;
	std::cout << "Order (Deploy) is Subject: "
			  << (dynamic_cast<Subject*>(deployOrder) != nullptr) << std::endl;
	std::cout << "OrdersList is Subject: "
			  << (dynamic_cast<Subject*>(&ordersList) != nullptr) << std::endl;
	std::cout << "GameEngine is Subject: "
			  << (dynamic_cast<Subject*>(&ge) != nullptr) << std::endl;

	std::cout << "\n-- Attaching logger and generating events --" << std::endl;

	// 1) CommandProcessor: saveCommand() should notify and write to gamelog
	cp.Attach(logger);
	cp.saveCommand(new Command("loadmap World.map"));

	// 2) Command: saveEffect() should notify and write to gamelog
	cmd.Attach(logger);
	cmd.saveEffect("Command effect example");

	// 3) OrdersList: addOrder() should notify and write to gamelog
	ordersList.Attach(logger);
	Player alice("Alice");
	Territory* aliceTerritory = new Territory("AliceTerritory", 1, 1);
	// create a deploy order with parameters, set player and add to orders list
	Deploy* d = new Deploy(&alice, aliceTerritory, 5);
	ordersList.addOrder(d);

	// 4) When an order is added to a player's orders list, observer is notified
	// Use Player's orders list and attach observer
	Player bob("Bob");
	OrdersList* bobOrders = bob.getOrdersList();
	bobOrders->Attach(logger);

	// Give Bob a reinforcement card so Deploy::validate() will succeed
	Card* reinfCard = new Card(CardType::REINFORCEMENT);
	Deck deck; // local deck required by Card::play
	bob.addCard(reinfCard);
	// Issue order by playing the card; this will add an order to bob's orders
	// list and notify
	bob.issueOrder(&deck, reinfCard);

	// Attach logger to the actual order so its execute() will notify
	Order* lastBobOrder = bob.getOrdersList()->getOrders().back();
	lastBobOrder->setPlayer(&bob);
	lastBobOrder->Attach(logger);

	// 5) When an order is executed, observer is notified and effect is logged
	lastBobOrder->execute();

	// 6) When GameEngine changes state, ensure the new state is output to the
	// log file
	ge.Attach(logger);
	ge.setState(StateType::playeradded);
	// GameEngine doesn't automatically call Notify in this implementation, so
	// call it explicitly
	ge.Notify(&ge);

	// Read and print the gamelog.txt contents to demonstrate results
	std::cout << "\n-- Contents of gamelog.txt --" << std::endl;
	std::ifstream in("gamelog.txt");
	if (in.is_open()) {
		std::string line;
		while (std::getline(in, line)) {
			std::cout << line << std::endl;
		}
		in.close();
	} else {
		std::cout << "Could not open gamelog.txt" << std::endl;
	}

	// Cleanup
	delete testPlayer;
	delete testTerritory;
	delete deployOrder;
	delete aliceTerritory;
	delete logger;
}
