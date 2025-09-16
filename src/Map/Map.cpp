#include "Map.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>

// Territory implementation
Territory::Territory(const std::string &name, int x, int y)
	: name(name), x(x), y(y), owner(nullptr), armies(0), continent(nullptr) {}

Territory::Territory(const Territory &other)
	: name(other.name), x(other.x), y(other.y), owner(other.owner),
	  armies(other.armies), adjacentTerritories(other.adjacentTerritories),
	  continent(other.continent) {}

Territory &Territory::operator=(const Territory &other) {
	if (this != &other) {
		name = other.name;
		x = other.x;
		y = other.y;
		owner = other.owner;
		armies = other.armies;
		adjacentTerritories = other.adjacentTerritories;
		continent = other.continent;
	}
	return *this;
}

void Territory::addAdjacency(Territory* territory) {
	if (std::find(adjacentTerritories.begin(), adjacentTerritories.end(),
				  territory) == adjacentTerritories.end()) {
		adjacentTerritories.push_back(territory);
	}
}

Territory::~Territory() = default;

std::ostream &operator<<(std::ostream &os, const Territory &territory) {
	os << "Territory: " << territory.name << " (" << territory.x << ","
	   << territory.y << ")";
	return os;
}

// Continent implementation
Continent::Continent(const std::string &name, int bonus)
	: name(name), controlBonus(bonus) {}

Continent::Continent(const Continent &other)
	: name(other.name), controlBonus(other.controlBonus),
	  territories(other.territories) {}

Continent &Continent::operator=(const Continent &other) {
	if (this != &other) {
		name = other.name;
		controlBonus = other.controlBonus;
		territories = other.territories;
	}
	return *this;
}

void Continent::addTerritory(Territory* territory) {
	if (std::find(territories.begin(), territories.end(), territory) ==
		territories.end()) {
		territories.push_back(territory);
		territory->setContinent(this);
	}
}

Continent::~Continent() = default;

std::ostream &operator<<(std::ostream &os, const Continent &continent) {
	os << "Continent: " << continent.name
	   << " (Bonus: " << continent.controlBonus
	   << ", Territories: " << continent.territories.size() << ")";
	return os;
}

// Map implementation
Map::Map() = default;

Map::~Map() = default;

Map::Map(const Map &other) {
	std::unordered_map<Territory*, Territory*>
		territoryMap; // First pass: create all territories
	for (const auto &territory : other.territories) {
		auto newTerritory = std::make_unique<Territory>(*territory);
		territoryMap[territory.get()] = newTerritory.get();
		territories.push_back(std::move(newTerritory));
	}

	// Second pass: recreate adjacencies using the mapping
	for (size_t i = 0; i < other.territories.size(); ++i) {
		const auto &oldAdjacencies = other.territories[i]->getAdjacencies();
		for (auto* oldAdj : oldAdjacencies) {
			territories[i]->addAdjacency(territoryMap[oldAdj]);
		}
	}

	// Recreate continents
	for (const auto &continent : other.continents) {
		auto newContinent = std::make_unique<Continent>(*continent);
		// Update territory pointers in the new continent
		for (auto* oldTerritory : continent->getTerritories()) {
			newContinent->addTerritory(territoryMap[oldTerritory]);
		}
		continents.push_back(std::move(newContinent));
	}
}

void Map::addTerritory(std::unique_ptr<Territory> territory) {
	territories.push_back(std::move(territory));
}

void Map::addContinent(std::unique_ptr<Continent> continent) {
	continents.push_back(std::move(continent));
}

Territory* Map::findTerritory(const std::string &name) {
	auto it =
		std::find_if(territories.begin(), territories.end(),
					 [&name](const auto &t) { return t->getName() == name; });
	return it != territories.end() ? it->get() : nullptr;
}

Continent* Map::findContinent(const std::string &name) {
	auto it =
		std::find_if(continents.begin(), continents.end(),
					 [&name](const auto &c) { return c->getName() == name; });
	return it != continents.end() ? it->get() : nullptr;
}

void Map::depthFirstSearch(Territory* start,
						   std::set<Territory*> &visited) const {
	visited.insert(start);
	for (Territory* adj : start->getAdjacencies()) {
		if (visited.find(adj) == visited.end()) {
			depthFirstSearch(adj, visited);
		}
	}
}

bool Map::isConnectedGraph() const {
	if (territories.empty())
		return true;

	std::set<Territory*> visited;
	depthFirstSearch(territories[0].get(), visited);

	return visited.size() == territories.size();
}

bool Map::areContinentsConnected() const {
	for (const auto &continent : continents) {
		if (continent->getTerritories().empty())
			continue;

		std::set<Territory*> visited;
		Territory* start = continent->getTerritories()[0];
		depthFirstSearch(start, visited);

		// Check if all territories in this continent are connected
		bool allConnected = std::all_of(
			continent->getTerritories().begin(),
			continent->getTerritories().end(), [&visited](Territory* t) {
				return visited.find(t) != visited.end();
			});

		if (!allConnected)
			return false;
	}
	return true;
}

bool Map::isTerritoryInOneContinent() const {
	for (const auto &territory : territories) {
		int continentCount = 0;
		for (const auto &continent : continents) {
			if (std::find(continent->getTerritories().begin(),
						  continent->getTerritories().end(), territory.get()) !=
				continent->getTerritories().end()) {
				continentCount++;
			}
		}
		if (continentCount != 1)
			return false;
	}
	return true;
}

bool Map::validate() const {
	return isConnectedGraph() && areContinentsConnected() &&
		   isTerritoryInOneContinent();
}

std::ostream &operator<<(std::ostream &os, const Map &map) {
	os << "Map with " << map.territories.size() << " territories and "
	   << map.continents.size() << " continents\n";

	os << "Territories:\n";
	for (const auto &territory : map.territories) {
		os << "  " << *territory << "\n";
		os << "    Adjacent to: ";
		for (const auto* adj : territory->getAdjacencies()) {
			os << adj->getName() << ", ";
		}
		os << "\n";
	}

	os << "Continents:\n";
	for (const auto &continent : map.continents) {
		os << "  " << *continent << "\n";
	}
	return os;
}

// MapLoader implementation
std::unique_ptr<Map> MapLoader::loadMap(const std::string &filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot open map file: " + filename);
	}

	auto map = std::make_unique<Map>();
	std::string line;
	std::string section;

	while (std::getline(file, line)) {
		// Skip empty lines and comments
		if (line.empty() || line[0] == ';')
			continue;

		// Check for section headers
		if (line[0] == '[') {
			section = line.substr(1, line.find(']') - 1);
			continue;
		}

		// Process sections
		if (section == "Continents") {
			if (line[0] !=
				'[') { // Process this line if it's not the section header
				std::istringstream iss(line);
				std::string name;
				std::string bonusStr;
				if (std::getline(iss, name, '=') &&
					std::getline(iss, bonusStr)) {
					try {
						int bonus = std::stoi(bonusStr);
						map->addContinent(
							std::make_unique<Continent>(name, bonus));
					} catch (const std::exception &) {
						throw std::runtime_error(
							"Error parsing continent bonus value");
					}
				}
			}
		} else if (section == "Territories") {
			if (line[0] !=
				'[') { // Process this line if it's not the section header
				std::istringstream iss(line);
				std::string item;
				std::vector<std::string> items;
				while (std::getline(iss, item, ',')) {
					items.push_back(item);
				}
				if (items.size() >= 4) {
					try {
						int x = std::stoi(items[1]);
						int y = std::stoi(items[2]);
						auto territory =
							std::make_unique<Territory>(items[0], x, y);

						// Find and set the continent
						Continent* continent = map->findContinent(items[3]);
						if (!continent) {
							throw std::runtime_error(
								"Invalid continent reference: " + items[3]);
						}
						continent->addTerritory(territory.get());

						// Store the territory and its adjacency info for later
						Territory* currTerritory = territory.get();
						map->addTerritory(std::move(territory));

						// Process adjacencies
						for (size_t i = 4; i < items.size(); ++i) {
							Territory* adjTerritory =
								map->findTerritory(items[i]);
							if (adjTerritory) {
								currTerritory->addAdjacency(adjTerritory);
								adjTerritory->addAdjacency(currTerritory);
							}
						}
					} catch (const std::exception &) {
						throw std::runtime_error(
							"Error parsing territory data");
					}
				}
			}
		}
	}

	if (!map->validate()) {
		throw std::runtime_error("Invalid map structure");
	}

	return map;
}

// These methods are now handled inline in loadMap
bool MapLoader::parseContinents(std::ifstream &, Map &) {
	return true;
}
bool MapLoader::parseTerritories(std::ifstream &, Map &) {
	return true;
}

bool MapLoader::validateMapFile(const std::string &filename) {
	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	bool hasMap = false, hasContinents = false, hasTerritories = false;
	std::string line;

	while (std::getline(file, line)) {
		if (line.empty())
			continue;
		if (line == "[Map]")
			hasMap = true;
		if (line == "[Continents]")
			hasContinents = true;
		if (line == "[Territories]")
			hasTerritories = true;
	}

	return hasMap && hasContinents && hasTerritories;
}