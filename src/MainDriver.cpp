#include "Map/Map.h"
#include "Player/Player.h"
#include "Utils/Utils.h"
#include "Map/MapDriver.h"
#include "GameEngine/GameEngineDriver.h"
#include <iostream>


int main() {
	std::cout << "Warzone Game Test Driver\n" << std::endl 
	<< SEPARATOR_LINE << std::endl
	<< "Map Tests\n" << std::endl;

	testLoadMaps();
	testGameStates();
	
	return 0;
}