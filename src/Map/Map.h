#pragma once
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>

typedef int* IntPtr;

// Forward declarations
class Player;
class Territory;
class Continent;
class Map;

class Territory {
  private:
	std::string* name;
	IntPtr x, y; // Coordinates on the map
	IntPtr nbOfArmies;
	std::vector<Territory*> adjacentTerritories;

  public:
	Territory(const std::string &name, int x, int y);
	Territory(const Territory &other);			  // Copy constructor
	Territory &operator=(const Territory &other); // Assignment operator
	~Territory();

	// Getters
	const std::string &getName() const;

	int getX() const;

	int getY() const;

	int getNbOfArmies() const;

	const std::vector<Territory*> &getAdjacentTerritories() const;

	// Setters
	void setName(const std::string &newName);

	void setX(int newX);

	void setY(int newY);

	void setNbOfArmies(int nb);

	void addAdjacentTerritory(Territory* t);
};

class Continent {
  private:
	std::string* name;
	int reinforcementBonus;
	std::vector<Territory*> territories;

  public:
	Continent(const std::string &name, int bonus);
	Continent(const Continent &other);			  // Copy constructor
	Continent &operator=(const Continent &other); // Assignment operator
	~Continent();

	void addTerritory(Territory* territory);

	// Basic getters
	const std::vector<Territory*> &getTerritories() const;
	int getReinforcementBonus() const;
	std::string getName() const;
};

class Map {
  private:
	bool* wrap;
    bool* warn;
  	std::string* author;
    std::string* image;
	std::string* name;
    std::string* scroll;
    std::vector<Territory*> territories;
    std::vector<Continent*> continents;

	void depthFirstSearch(Territory* start, std::unordered_set<Territory*>& visited) const;
	void isConnectedGraph() const;
	bool areContinentsConnected() const;
	bool isTerritoryInOneContinent() const;

  public:
	Map(const Map &other);			  // Copy constructor
	Map(bool wrap, bool warn, const std::string& author, 
		const std::string &image, const std::string &name, 
		const std::string &scroll);
	Map &operator=(const Map &other); // Assignment operator
	~Map();

	// Getters
	bool getWrap() const;
    bool getWarn() const;
    std::string getAuthor() const;
	std::string getImage() const;
	std::string getName() const;
    std::string getScroll() const;
	const std::vector<Continent*>& getContinents() const;

	void addTerritory(Territory* territory);
    void addContinent(Continent* continent);
	const std::vector<Continent*>& getContinents() const;

	bool validate() const;
};
