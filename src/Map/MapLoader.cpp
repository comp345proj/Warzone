#include "MapLoader.h"
#include "Utils/Utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

// ---------- trim helpers -----------------
namespace {
std::string ltrim(std::string s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
				return !std::isspace(ch);
			}));
	return s;
}

std::string rtrim(std::string s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
						 [](unsigned char ch) { return !std::isspace(ch); })
				.base(),
			s.end());
	return s;
}

std::string trim(std::string s) {
	s = ltrim(s);
	s = rtrim(s);
	return s;
}
} // namespace

// ---------- MapLoader class implementation -----------------

MapLoader::MapLoader(const std::string &fname)
	: filename(new std::string(fname)) {}

MapLoader::~MapLoader() {
	delete filename;
}

// In [Map] section, convert "yes" to true, "no" to false
bool MapLoader::convertStrToBool(const std::string &str) {
	return str == "yes";
}

Map* MapLoader::loadMap() {
	std::ifstream file(*filename);
	std::string line;

	if (!file.is_open()) {
		std::cerr << "Could not open the file: " << filename << std::endl;
		return nullptr;
	}

	std::string worldName = filename->substr(0, filename->size() - 4);

	bool wrap = false;
	bool warn = false;
	std::string author = "";
	std::string image = "";
	std::string scroll = "";

	std::cout << "Starting to read map file..." << std::endl;
	while (std::getline(file, line)) {
		line = trim(line);
		if (line.empty())
			continue;

		if (line == MAP_SECTION_HEADER)
			continue;
		// Stop at Continents section
		if (line == CONTINENT_SECTION_HEADER)
			break;

		getMapDetails(line, wrap, warn, author, image, scroll);
	}

	std::cout << "Done reading [Map] section." << std::endl;

	Map* map = new Map(wrap, warn, author, image, worldName, scroll);

	std::cout << "Created Map: " << map->getName() << std::endl
			  << "Author: " << map->getAuthor() 
			  << ", Image: " << map->getImage() << std::endl;

	std::cout << "Starting to parse continents..." << std::endl;
	// Parsing Continents
	while (std::getline(file, line)) {
		line = trim(line);
		if (line.empty())
			continue;
		if (line == TERRITORY_SECTION_HEADER) {
			std::cout << "Finished parsing continents, moving to territories..." << std::endl;
			break;
		}

		std::string continentName;
		int bonus;

		getContinentDetails(line, continentName, bonus);
		Continent* continent = new Continent(continentName, bonus);

		std::cout << "Created Continent: " << continent->getName() << std::endl;
		map->addContinent(continent);
	}

	// Create a map of continent names to continent pointers for easy lookup
	//  This is needed to associate territories with correct continents
	std::unordered_map<std::string, Continent*> nameToContinent;

	// Associate continent names to continent pointers
	for (Continent* continent : map->getContinents()) {
		nameToContinent[continent->getName()] = continent;
	}

	// Create a map of territory names to territory pointers
	// This is needed to associate adjacent territories correctly
	std::unordered_map<std::string, Territory*> nameToTerritory;

	// This will be later  used to associate territories with their adjacent territories
	//  after all territories have been created
	std::unordered_map<Territory*, std::vector<std::string>> territoryToAdjacentNames;

	std::cout << "Starting to parse territories..." << std::endl;
	while (std::getline(file, line)) {
		line = trim(line);
		if (line.empty())
			continue;

		std::cout << "Processing territory: " << line << std::endl;
		std::istringstream ss(line);
		std::string name, posX, posY, continentName;
		// Split first 4 values by ','
		std::getline(ss, name, ',');
		std::getline(ss, posX, ',');
		std::getline(ss, posY, ',');
		std::getline(ss, continentName, ',');

		name = trim(name);
		int x = std::stoi(posX);
		int y = std::stoi(posY);
		continentName = trim(continentName);

		Territory* territory = new Territory(name, x, y);
		// Add the territory to name-to-territory 
		// map for looking up later (to associate with correct continent)
		nameToTerritory[name] = territory;

		// Add territory to the map
		map->addTerritory(territory);

		// Associate territory with correct continent
		//  If continent not found, print error
		//  If found, add territory to continent's territory list
		if (nameToContinent.find(continentName) != nameToContinent.end()) {
			Continent* continent = nameToContinent[continentName];
			continent->addTerritory(territory);
		} else {
			// error handling if territory doesn't exist
			std::cout << "**ERROR**: Continent " << continentName
					  << " for Territory " << name << " is not found."
					  << std::endl;
		}


		// After adding the territory continent, parse the rest of the line
		//  The rest of the line are adjacent territories, separated by ','
		std::vector<std::string> adjacentNames;
		std::string adjacentName;
		// Read remaining adjacent territory names
		//  and store them in a map of territory to its adjacent names
		while (std::getline(ss, adjacentName, ',')) {
			adjacentNames.push_back(trim(adjacentName));
		}
		//< *terr -> [adj1, adj2, ...], ... >
		territoryToAdjacentNames[territory] = adjacentNames;
	}

	// Now that all territories are created, associate adjacent territories
	//  by looking up the names in nameToTerritory map

	// use "auto" to avoid long type declaration
	for (auto &pair : territoryToAdjacentNames) {
		// get the key from the K-V pair
		Territory* territory = pair.first;
		// for each adjacent territory name, look up the territory pointer
		//  and add it to the territory's adjacent list
		for (const std::string &adjName : pair.second) {
			if (nameToTerritory.find(adjName) != nameToTerritory.end()) {
				Territory* adjacentTerritory = nameToTerritory[adjName];
				territory->addAdjacentTerritory(adjacentTerritory);
			} else {
				// Error handling if adjacent territory doesn't exist
				std::cout << "**ERROR**: Adjacent Territory " << adjName
						  << " for Territory " << territory->getName()
						  << " is not found." << std::endl;
			}
		}
	}

	file.close();
	return map;
}

// Helper functions to parse lines in [Map] 
// Split line at '=', trim spaces, and assign to correct variables
// substr (0, pos) is key | substr (pos+1) to end is value
void MapLoader::getMapDetails(const std::string line, bool &wrap, bool &warn,
							  std::string &author, std::string &image,
							  std::string &scroll) {
	size_t pos = line.find('=');
	if (pos == std::string::npos) {
		return; // not a key=value line
	}

	std::string key = trim(line.substr(0, pos));
	std::string value = trim(line.substr(pos + 1));

	if (key == "author") {
		author = value;
	} else if (key == "image") {
		image = value;
	} else if (key == "wrap") {
		wrap = convertStrToBool(value);
	} else if (key == "scroll") {
		scroll = value;
	} else if (key == "warn") {
		warn = convertStrToBool(value);
	}
}

// In [Continents] section, split line at '=', trim spaces,
// assign left side to name, right side to bonus (convert to int)
void MapLoader::getContinentDetails(const std::string line, std::string &name,
									int &bonus) {
	size_t pos = line.find('=');
	if (pos == std::string::npos) {
		name.clear();
		bonus = 0;
		return;
	}

	name = trim(line.substr(0, pos));
	std::string bonusStr = trim(line.substr(pos + 1));

	try {
		bonus = std::stoi(bonusStr);
	} catch (...) {
		bonus = 0;
	}
}
