#include "MapLoader.h"
#include "Utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <sstream>

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
		std::cerr << "Could not open the file: " << *filename << std::endl;
		return nullptr;
	}

	std::string worldName = filename->substr(0, filename->size() - 4);

	bool* wrap;
	bool* warn;
	std::string* author;
	std::string* image;
	std::string* scroll;

	while (std::getline(file, line)) {
		line = trim(line);
		if (line.empty())
			continue;

		if (line == MAP_SECTION_HEADER)
			continue;
		// Stop at Continents section
		if (line == CONTINENT_SECTION_HEADER)
			break;

		getMapDetails(line, *wrap, *warn, *author, *image, *scroll);
	}

	Map* map = new Map(*wrap, *warn, *author, *image, worldName, *scroll);


	// Parsing Continents
	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty()) continue;
		if (line == TERRITORY_SECTION_HEADER) break;

		std::string* continentName;
		IntPtr bonus;

		getContinentDetails(line, *continentName, *bonus);
		Continent* continent = new Continent(*continentName, *bonus);
		map->addContinent(continent);
	}
	
	//Create a map of continent names to continent pointers for easy lookup
    std::unordered_map<std::string, Continent*> nameToContinent;

	for (Continent* cont : map->getContinents()) {
		nameToContinent[cont->getName()] = cont;
    }

	// Map of territory names to territory pointers
	std::unordered_map<std::string, Territory*> nameToTerritory;

	while(std::getline(file, line)){
		line = trim(line);
		if (line.empty()) continue;
		
		std::istringstream ss(line);
        std::string name, x_str, y_str, continentName;
        std::getline(ss, name, ',');
        std::getline(ss, x_str, ',');
        std::getline(ss, y_str, ',');
        std::getline(ss, continentName, ',');

        name = trim(name);
        int x = std::stoi(x_str);
        int y = std::stoi(y_str);
        continentName = trim(continentName);

        Territory* territory = new Territory(name, x, y);
        //Add the teirrotiry to name-to-territory map for looking up later (to associate with correct continent)
        nameToTerritory[name] = territory;

        //Add territory to the map
        map->addTerritory(territory);
	}
}

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

void MapLoader::getContinentDetails(const std::string line, std::string &name,
									int &bonus) {
	size_t pos = line.find('=');
	if (pos == std::string::npos) {
		return; // not a key=value line
	}

	std::string key = trim(line.substr(0, pos));
	std::string value = trim(line.substr(pos + 1));

	name = trim(name);

	if (key == "name") {
		name = value;
	} else if (key == "bonus") {
		try {
			bonus = std::stoi(trim(value));
		} catch (const std::invalid_argument &e) {
			bonus = 0; // default to 0 if conversion fails
		}
	}
}

