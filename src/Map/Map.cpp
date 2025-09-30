#include <iostream>
#include <string>
#include <vector>
#include "Map.h"
#include <unordered_map>

//---------------------------Territory-------------------------------

Territory::Territory(const std::string& name, int posX, int posY){
    this->name = new std::string(name);
    this->x = new int(posX);
    this->y = new int(posY);
    this->nbOfArmies = new int (0);
}

Territory::~Territory(){
    delete this->name;
    delete this->x;
    delete this->y;
    delete this->nbOfArmies;
    this->name = nullptr;
    this->x = nullptr;
    this->y = nullptr;
    this->nbOfArmies = nullptr;
}

Territory::Territory(const Territory& territory2) {
    this->name = new std::string(*(territory2.name));
    this->x = new int(*(territory2.x));
    this->y = new int(*(territory2.y));
    this->adjacentTerritories = territory2.adjacentTerritories; 
}


void Territory::addAdjacentTerritory(Territory* territory){
    adjacentTerritories.push_back(territory);
}

std::string Territory::getName() const{
    return *name;
}
int Territory::getX() const{
    return *x;
}
int Territory::getY() const{
    return *y;
}

int Territory::getNbOfArmies() const {
    return *nbOfArmies;
}

// Getter for the adjacent territories.
const std::vector<Territory*>& Territory::getAdjacentTerritories() const {
    return adjacentTerritories;
}

//Basic setters
void Territory::setName(const std::string& n){
    delete this->name;
    this->name = new std::string(n);
}
void Territory::setX (int posX){
    delete this->x;
    this->x = new int(posX);
}
void Territory::setY (int posY){
    delete this->y;
    this->y = new int(posY);
}

void Territory::setNbOfArmies(int numOfArmies) {
    delete this->nbOfArmies;
    this->nbOfArmies = new int(numOfArmies);
}

//Assignment Operator
Territory& Territory::operator=(const Territory& territory2) {
    if (this == &territory2) return *this;
    delete name;
    delete x;
    delete y;
    name = new std::string(*(territory2.name));
    x = new int(*(territory2.x));
    y = new int(*(territory2.y));
    adjacentTerritories = territory2.adjacentTerritories;
    return *this;
}

//Stream insertion Operator
std::ostream& operator<<(std::ostream& out, const Territory& territory) {
    out << "Territory Name: " << territory.getName() 
    << ", X: " << territory.getX() 
    << ", Y: " << territory.getY() 
    << ", Number of armies: " << territory.getNbOfArmies();
    return out;
}





//-------------------------------Continent-----------------------------

Continent::Continent(const std::string& name, int reinforcementBonus) {
    this->name = new std::string(name);
    this->reinforcementBonus = new int(reinforcementBonus);
}

Continent::~Continent(){
    delete this->name;
    delete this->reinforcementBonus;
    this->name = nullptr;
    this->reinforcementBonus = nullptr;
}

Continent::Continent(const Continent& continent2) {
    this->name = new std::string(*(continent2.name));
    this->reinforcementBonus = new int(*(continent2.reinforcementBonus));
    this->territories = continent2.territories;
}


// Adds a territory to the current continent's territories vector.
void Continent::addTerritory(Territory* territory) {
    this->territories.push_back(territory);
}

//Basic getters
const std::vector<Territory*>& Continent::getTerritories() const {
    return this->territories;
}
int Continent::getReinforcementBonus() const {
    return *this->reinforcementBonus;
}
std::string Continent::getName() const {
    return *this->name;
}

//Assignment Operator
Continent& Continent::operator=(const Continent& continent2) {
    if (this == &continent2) return *this;
    delete this->name;
    delete this->reinforcementBonus;
    this->name = new std::string(*(continent2.name));
    this->reinforcementBonus = new int(*(continent2.reinforcementBonus));
    this->territories = continent2.territories;
    return *this;
}

//Stream insertion Operator
std::ostream& operator<<(std::ostream& out, const Continent& continent) {
    out << "Continent Name: " << continent.getName() 
    << ", Number of Armies: " << continent.getReinforcementBonus();
    return out;
}


//---------------------------------Map----------------------------------

Map::Map(
    bool wrap, 
    bool warn, 
    const std::string& author, 
    const std::string& image, 
    const std::string& name, 
    const std::string& scroll
){
    this->wrap = new bool(wrap);
    this->warn = new bool(warn);
    this->author = new std::string(author);
    this->image = new std::string(image);
    this->name = new std::string(name);
    this->scroll = new std::string(scroll);
}

Map::~Map(){
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

    for (Territory* territory : territories){
        delete territory;
    }
    territories.clear();

    for (Continent* continent : continents){
        delete continent;
    }
    territories.clear();
}

// Copy constructor Just copying the pointers, not the territories and continents themselves as it doesn't make sense.
Map::Map(const Map& map2) {
    this->image = new std::string(*(map2.image));
    this->scroll = new std::string(*(map2.scroll));
    this->author = new std::string(*(map2.author));
    this->name = new std::string(*(map2.name));
    this->wrap = new bool(*(map2.wrap));
    this->warn = new bool(*(map2.warn));

    this->territories = map2.territories;
    this->continents = map2.continents;
}


// Adds a territory to the map's territories vector.
void Map::addTerritory(Territory* territory){
    this->territories.push_back(territory);
}

// Adds a continent to the map's continents vector.
void Map::addContinent(Continent* continent){
    this->continents.push_back(continent);
}

// Basic getters
std::string Map::getImage() const{
    return *image;
}
std::string Map::getScroll() const{
    return *scroll;
}
std::string Map::getAuthor() const{
    return *author;
}
std::string Map::getName() const{
    return *name;
}
bool Map::getWrap() const {
    return *wrap;
}
bool Map::getWarn() const {
    return *warn;
}
const std::vector<Continent*>& Map::getContinents() const {
    return continents;
}

//Assignment Operator
Map& Map::operator=(const Map& map2) {
    if (this == &map2) return *this;
    delete image;
    delete scroll;
    delete author;
    delete name;
    delete wrap;
    delete warn;
    this->image = new std::string(*(map2.image));
    this->scroll = new std::string(*(map2.scroll));
    this->author = new std::string(*(map2.author));
    this->name = new std::string(*(map2.name));
    this->wrap = new bool(*(map2.wrap));
    this->warn = new bool(*(map2.warn));
    this->territories = map2.territories;
    this->continents = map2.continents;
    return *this;
}


//Stream insertion Operator
std::ostream& operator<<(std::ostream& out, const Map& map) {
    out << "Map Name: " << map.getName() 
    << ", Author: " << map.getAuthor();
    return out;
}


//--------------------------------Map Validation Helper methods----------------------------------
// Depth-first search to visit territories and build the visited set.
void Map::depthFirstSearch(Territory* start, 
    std::unordered_set<Territory*>& visited) const {
    if (visited.find(start) != visited.end()) return;

    visited.insert(start);
    for (Territory* adjacent : start->getAdjacentTerritories()) {
        depthFirstSearch(adjacent, visited);
    }
}

// Checks if all territories in the map are interconnected.
//Satisfies: 1) the map is a connected graph
bool Map::isConnectedGraph() const {
    if (territories.empty()) return true;
    
    std::unordered_set<Territory*> visited;
    depthFirstSearch(territories[0], visited);

    return visited.size() == territories.size();
}

// Checks if all continents in the map are interconnected through their territories.
//Satisfies: 2) continents are connected subgraphs
bool Map::areContinentsConnected() const {
    if (continents.empty() || territories.empty()) return true;

    std::unordered_set<Territory*> visited;
    depthFirstSearch(territories[0], visited);

    for (Continent* continent : continents) {
        bool continentVisited = false;

        for (Territory* territory : continent->getTerritories()) {
            if (visited.find(territory) != visited.end()) {
                continentVisited = true;
                break;
            }
        }

        if (!continentVisited) return false;
    }

    return true;
}

// Checks if territories are unique to continents.
//Satisfies: 3) each country belongs to one and only one continent
bool Map::isTerritoryInOneContinent() const {
    std::unordered_set<Territory*> territoriesSet;

    for (Continent* continent : continents) {
        for (Territory* territory : continent->getTerritories()) {
            if (territoriesSet.find(territory) != territoriesSet.end()) return false;
            territoriesSet.insert(territory);
        }
    }
    return true;
}

// Determines the validity of the map.
bool Map::validate() const {
    //std::cout << isConnected() << " " << areContinentsInterconnected() << " " << areTerritoriesUniqueToContinents() << " these were results\n";
    std::cout << "VALIDATION RESULTS:\n" <<
    "1) The map is a connected graph: " << (isConnectedGraph() ? "true" : "false") << "\n" <<
    "2) All continents are connected subgraphs: " << (areContinentsConnected() ? "true" : "false") << "\n" <<
    "3) Each territory belongs to one and only one continent: " << (isTerritoryInOneContinent() ? "true" : "false") << "\n";
    return isConnectedGraph() && areContinentsConnected() && isTerritoryInOneContinent();
}

