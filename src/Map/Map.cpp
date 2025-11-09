#include "Map.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

//---------------------------Territory-------------------------------

// Constructor with parameters for name and coordinates.
// Initializes number of armies to 0
Territory::Territory(const std::string &name, int posX, int posY)
    : name(new std::string(name)), x(new int(posX)), y(new int(posY)),
      armies(new int(0)), player(nullptr) {}

// Destructor to clean up dynamically allocated memory.
Territory::~Territory() {
    delete name;
    delete x;
    delete y;
    delete armies;
    name = nullptr;
    x = nullptr;
    y = nullptr;
    armies = nullptr;
}

// Copy constructor
Territory::Territory(const Territory &territory2) {
    name = new std::string(*(territory2.name));
    x = new int(*(territory2.x));
    y = new int(*(territory2.y));
    adjacentTerritories = territory2.adjacentTerritories;
    armies = new int(*(territory2.armies));
}

// Adds an adjacent territory to the current territory's adjacency list.
void Territory::addAdjacentTerritory(Territory* territory) {
    adjacentTerritories.push_back(territory);
}

/*
 ** Getters for name, coordinates, number of armies, and adjacent territories.
 * **
 */
std::string Territory::getName() const {
    return *name;
}
int Territory::getX() const {
    return *x;
}
int Territory::getY() const {
    return *y;
}

const std::vector<Territory*> &Territory::getAdjacentTerritories() const {
    return adjacentTerritories;
}

Player* Territory::getPlayer() {
    return player;
}

// Basic setters
void Territory::setName(const std::string &n) {
    delete name;
    name = new std::string(n);
}
void Territory::setX(int posX) {
    delete x;
    x = new int(posX);
}
void Territory::setY(int posY) {
    delete y;
    y = new int(posY);
}

void Territory::setArmies(int numOfArmies) {
    delete armies;
    armies = new int(numOfArmies);
}

void Territory::setPlayer(Player* p) {
    player = p;
}

// Assignment Operator
// Custom Operator which handles self-assignment and deep copies the data.
Territory &Territory::operator=(const Territory &territory2) {
    if (this == &territory2)
        return *this;
    delete name;
    delete x;
    delete y;
    delete armies;
    name = new std::string(*(territory2.name));
    x = new int(*(territory2.x));
    y = new int(*(territory2.y));
    adjacentTerritories = territory2.adjacentTerritories;
    armies = new int(*(territory2.armies));
    player = territory2.player;
    return *this;
}

// Stream insertion Operator
// Custom << operator to print the territory's details.
std::ostream &operator<<(std::ostream &out, const Territory &territory) {
    out << "Territory Name: " << territory.getName()
        << ", X: " << territory.getX() << ", Y: " << territory.getY()
        << ", Number of armies: " << territory.getArmies();
    return out;
}

//-------------------------------Continent-----------------------------
// Constructor with parameters for name and reinforcement bonus.
Continent::Continent(const std::string &name, int reinforcementBonus)
    : name(new std::string(name)),
      reinforcementBonus(new int(reinforcementBonus)) {}

// Copy constructor
Continent::Continent(const Continent &continent2) {
    name = new std::string(*(continent2.name));
    reinforcementBonus = new int(*(continent2.reinforcementBonus));
    territories = continent2.territories;
}

// Custom Destructor to clean up dynamically allocated memory.
// territories vector is not deleted here, will be handled by Map class
// destructor.
Continent::~Continent() {
    delete name;
    delete reinforcementBonus;
    name = nullptr;
    reinforcementBonus = nullptr;
}

// Adds a territory to the current continent's territories vector.
void Continent::addTerritory(Territory* territory) {
    territories.push_back(territory);
}

// Basic getters
const std::vector<Territory*> &Continent::getTerritories() const {
    return territories;
}
int Continent::getReinforcementBonus() const {
    return *reinforcementBonus;
}
std::string Continent::getName() const {
    return *name;
}

// Assignment Operator, handles self-assignment and deep copies the data.
Continent &Continent::operator=(const Continent &continent2) {
    if (this == &continent2)
        return *this;
    delete name;
    delete reinforcementBonus;
    name = new std::string(*(continent2.name));
    reinforcementBonus = new int(*(continent2.reinforcementBonus));
    territories = continent2.territories;
    return *this;
}

// Stream insertion Operator for printing continent details.
std::ostream &operator<<(std::ostream &out, const Continent &continent) {
    out << "Continent Name: " << continent.getName()
        << ", Territories: " << continent.getTerritories().size()
        << ", Bonus: " << continent.getReinforcementBonus();
    return out;
}

//---------------------------------Map----------------------------------
// Constructor with parameters for map properties.
Map::Map(bool wrap,
         bool warn,
         const std::string &author,
         const std::string &image,
         const std::string &name,
         const std::string &scroll)
    : wrap(new bool(wrap)), warn(new bool(warn)),
      author(new std::string(author)), image(new std::string(image)),
      name(new std::string(name)), scroll(new std::string(scroll)) {}

// Custom Destructor to clean up dynamically allocated memory.
// Also deletes all territories and continents associated with the map.
Map::~Map() {
    delete image;
    delete scroll;
    delete author;
    delete name;
    delete wrap;
    delete warn;
    image = nullptr;
    scroll = nullptr;
    author = nullptr;
    name = nullptr;
    wrap = nullptr;
    warn = nullptr;

    for (Territory* territory : territories) {
        delete territory;
    }
    territories.clear();

    for (Continent* continent : continents) {
        delete continent;
    }
    territories.clear();
}

// Copy constructor for deep copying map properties and references to
// territories and continents.
Map::Map(const Map &map2) {
    image = new std::string(*(map2.image));
    scroll = new std::string(*(map2.scroll));
    author = new std::string(*(map2.author));
    name = new std::string(*(map2.name));
    wrap = new bool(*(map2.wrap));
    warn = new bool(*(map2.warn));

    territories = map2.territories;
    continents = map2.continents;
}

// Adds a territory to the map's territories vector.
void Map::addTerritory(Territory* territory) {
    territories.push_back(territory);
}

// Adds a continent to the map's continents vector.
void Map::addContinent(Continent* continent) {
    continents.push_back(continent);
}

// Basic getters
std::string Map::getImage() const {
    return *image;
}
std::string Map::getScroll() const {
    return *scroll;
}
std::string Map::getAuthor() const {
    return *author;
}
std::string Map::getName() const {
    return *name;
}
bool Map::getWrap() const {
    return *wrap;
}
bool Map::getWarn() const {
    return *warn;
}
const std::vector<Continent*> &Map::getContinents() const {
    return continents;
}

// Assignment Operator
Map &Map::operator=(const Map &map2) {
    if (this == &map2)
        return *this;
    delete image;
    delete scroll;
    delete author;
    delete name;
    delete wrap;
    delete warn;
    image = new std::string(*(map2.image));
    scroll = new std::string(*(map2.scroll));
    author = new std::string(*(map2.author));
    name = new std::string(*(map2.name));
    wrap = new bool(*(map2.wrap));
    warn = new bool(*(map2.warn));
    territories = map2.territories;
    continents = map2.continents;
    return *this;
}

// Stream insertion Operator
std::ostream &operator<<(std::ostream &out, const Map &map) {
    out << "Map Name: " << map.getName() << ", Author: " << map.getAuthor();
    return out;
}

//----------------------- Map Validation Methods -----------------------
// Depth-first search to visit territories and build the visited set.
void Map::depthFirstSearch(Territory* start,
                           std::unordered_set<Territory*> &visited) const {
    // Base case: if already visited, return.
    if (visited.find(start) != visited.end())
        return;
    // Mark the current territory as visited.
    visited.insert(start);
    for (Territory* adjacent : start->getAdjacentTerritories()) {
        depthFirstSearch(adjacent, visited);
    }
}

// Checks if all territories in the map are interconnected.
// Satisfies: 1) the map is a connected graph
bool Map::isConnectedGraph() const {
    if (territories.empty())
        return true;

    std::unordered_set<Territory*> visited;
    depthFirstSearch(territories[0], visited);

    // If the number of visited territories equals total territories, it's
    // connected.
    return visited.size() == territories.size();
}

// Checks if all continents in the map are interconnected through their
// territories. Satisfies: 2) all continents are connected subgraphs
bool Map::areContinentsConnected() const {
    if (continents.empty() || territories.empty())
        return true;

    // Perform DFS from the first territory and see if we can reach at least one
    // territory in each continent.
    std::unordered_set<Territory*> visited;
    depthFirstSearch(territories[0], visited);

    // Loops through each continent to check if at least one territory has been
    // visited. If any continent has no visited territories, return false.
    for (Continent* continent : continents) {
        bool continentVisited = false;

        for (Territory* territory : continent->getTerritories()) {
            if (visited.find(territory) != visited.end()) {
                continentVisited = true;
                break;
            }
        }

        if (!continentVisited)
            return false;
    }

    return true;
}

// Checks if territories are unique to continents.
// Satisfies: 3) each country belongs to one and only one continent
bool Map::isTerritoryInOneContinent() const {
    std::unordered_set<Territory*> territoriesSet;

    for (Continent* continent : continents) {
        for (Territory* territory : continent->getTerritories()) {
            if (territoriesSet.find(territory) != territoriesSet.end())
                return false;
            territoriesSet.insert(territory);
        }
    }
    return true;
}

// Determines the validity of the map.
bool Map::validate() const {
    // std::cout << isConnected() << " " << areContinentsInterconnected() << " "
    // << areTerritoriesUniqueToContinents() << " these were results\n";
    std::cout << "VALIDATION RESULTS:\n"
              << "1) The map is a connected graph: "
              << (isConnectedGraph() ? "true" : "false") << "\n"
              << "2) All continents are connected subgraphs: "
              << (areContinentsConnected() ? "true" : "false") << "\n"
              << "3) Each territory belongs to one and only one continent: "
              << (isTerritoryInOneContinent() ? "true" : "false") << "\n";
    return isConnectedGraph() && areContinentsConnected()
        && isTerritoryInOneContinent();
}
