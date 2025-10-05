#include "Cards.h"
#include <iostream>

void testCards() {
	std::cout << "Testing Cards functionality" << std::endl;
	std::cout << "=========================" << std::endl;

	// Create a deck with all types of cards
	std::cout << "\nCreating and initializing deck with all card types:"
			  << std::endl;
	Deck deck;
	std::cout << deck;

	// Demonstrate that deck contains all types of cards
	std::cout << "\nDrawing one of each card type to demonstrate deck contents:"
			  << std::endl;
	std::unordered_map<CardType, bool> cardTypesSeen;
	Hand tempHand;
	while (cardTypesSeen.size() < 5 && deck.size() > 0) {
		Card* card = deck.draw();
		if (card) {
			if (!cardTypesSeen[card->getType()]) {
				std::cout << "Found card type: " << *card << std::endl;
				cardTypesSeen[card->getType()] = true;
			}
			tempHand.addCard(card);
		}
	}

	// Return demonstration cards to deck
	std::cout << "\nReturning demonstration cards to deck..." << std::endl;
	auto tempCards = tempHand.getCards();
	for (Card* card : tempCards) {
		deck.returnCard(card);
		tempHand.removeCard(card);
	}

	// Create a hand for actual testing
	std::cout << "\nCreating a new hand and drawing 5 cards:" << std::endl;
	Hand hand;
	const int cardsToDraw = 5;
	for (int i = 0; i < cardsToDraw; ++i) {
		Card* card = deck.draw();
		if (card) {
			hand.addCard(card);
			std::cout << "Drew: " << *card << std::endl;
		}
	}

	std::cout << "\nHand now contains:" << std::endl;
	std::cout << hand;
	std::cout << "Deck has " << deck.size() << " cards remaining" << std::endl;

	std::cout << "Creating a test player to play the hand" << std::endl;
	Player testPlayer("TestPlayer");
	testPlayer.setHand(&hand);

	// Play all cards in hand
	std::cout << "\nPlaying all test player cards from hand:" << std::endl;
	auto cardsInHand = testPlayer.getCards();

	for (Card* card : cardsInHand) {

		std::cout << "\nPlaying " << *card << " card" << std::endl;

		// Playing a card creates order, removes card from hand,
		// and returns to deck, atomic operation
		card->play(&testPlayer, &deck);

		std::cout << "Card played and returned to deck, deck now has "
				  << deck.size() << " cards" << std::endl;
	}

	// Show final state
	std::cout << "\nFinal hand state (should be empty):" << std::endl;
	std::cout << hand;
	std::cout << "\nFinal deck state (should have all cards back):"
			  << std::endl;
	std::cout << deck;

	std::cout << "\nTest completed successfully!" << std::endl;
}