#pragma once
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class Player;
class Territory;
class Continent;
class Map;

class Territory {
  public:
	Territory(const std::string &name, int x, int y);
	Territory(const Territory &other);			  // Copy constructor
	Territory &operator=(const Territory &other); // Assignment operator
	~Territory();

	// Getters
	const std::string &getName() const {
		return name;
	}
	const std::vector<Territory*> &getAdjacencies() const {
		return adjacentTerritories;
	}
	Continent* getContinent() const {
		return continent;
	}
	Player* getOwner() const {
		return owner;
	}
	int getArmies() const {
		return armies;
	}
	int getX() const {
		return x;
	}
	int getY() const {
		return y;
	}

	// Setters
	void addAdjacency(Territory* territory);
	void setContinent(Continent* cont) {
		continent = cont;
	}
	void setOwner(Player* player) {
		owner = player;
	}
	void setArmies(int count) {
		armies = count;
	}

	friend std::ostream &operator<<(std::ostream &os,
									const Territory &territory);

  private:
	std::string name;
	int x, y; // Coordinates on the map
	Player* owner;
	int armies;
	std::vector<Territory*> adjacentTerritories;
	Continent* continent;
};

class Continent {
  public:
	Continent(const std::string &name, int bonus);
	Continent(const Continent &other);			  // Copy constructor
	Continent &operator=(const Continent &other); // Assignment operator
	~Continent();

	// Getters
	const std::string &getName() const {
		return name;
	}
	int getBonus() const {
		return controlBonus;
	}
	const std::vector<Territory*> &getTerritories() const {
		return territories;
	}

	// Setters
	void addTerritory(Territory* territory);

	friend std::ostream &operator<<(std::ostream &os,
									const Continent &continent);

  private:
	std::string name;
	int controlBonus;
	std::vector<Territory*> territories;
};

class Map {
  public:
	Map();
	Map(const Map &other);			  // Copy constructor
	Map &operator=(const Map &other); // Assignment operator
	~Map();

	// Map validation methods
	bool validate() const;
	bool isConnectedGraph() const;
	bool areContinentsConnected() const;
	bool isTerritoryInOneContinent() const;

	// Getters
	const std::vector<std::unique_ptr<Territory>> &getTerritories() const {
		return territories;
	}
	const std::vector<std::unique_ptr<Continent>> &getContinents() const {
		return continents;
	}

	// Setters
	void addTerritory(std::unique_ptr<Territory> territory);
	void addContinent(std::unique_ptr<Continent> continent);
	Territory* findTerritory(const std::string &name);
	Continent* findContinent(const std::string &name);

	friend std::ostream &operator<<(std::ostream &os, const Map &map);

  private:
	std::vector<std::unique_ptr<Territory>> territories;
	std::vector<std::unique_ptr<Continent>> continents;
	void depthFirstSearch(Territory* start,
						  std::set<Territory*> &visited) const;
};

class MapLoader {
  public:
	static std::unique_ptr<Map> loadMap(const std::string &filename);

  private:
	static bool validateMapFile(const std::string &filename);
	static bool parseContinents(std::ifstream &file, Map &map);
	static bool parseTerritories(std::ifstream &file, Map &map);
};
