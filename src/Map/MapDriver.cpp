#include <iostream>
#include <vector>
#include "MapDriver.h"
#include "Map.h"
#include "MapLoader.h"
#include "Utils/Utils.h"

void testLoadMaps()
{
    std::vector<std::string> mapFiles = {
        "../res/World.map",
        "../res/invalid.map",
        // "maps/Europe.map",
        // "maps/Canada.map",
        // "maps/InvalidAdjacency.map", // Invalid adjacency
        // "maps/InvalidContinentBonus.map", // Invalid continent bonus
        // "maps/InvalidTerritoryContinent.map", // Territory with non-existent continent
        // "maps/InvalidTerritoryAdjacency.map", // Territory with non-existent adjacency
        // "maps/InvalidFormat.map" // Invalid format
    };

    std::vector<Map*> loadedValidMaps;

    for (int i = 0; i < mapFiles.size(); i++)
    {
        MapLoader loader(mapFiles[i]);
        Map* map = loader.loadMap();
        if (!map) {
            std::cout << SEPARATOR_LINE << std::endl;
            std::cerr << "Failed to load map from file: " << mapFiles[i] << std::endl;
            continue;
        }

        
        std::cout << "Testing map file: " << map->getName() << std::endl;

        std::cout << "Map Details:" << std::endl;
        std::cout << "Author: " << map->getAuthor() << std::endl
        << "Image: " << map->getImage() << std::endl
        << "Wrap: " << (map->getWrap() ? "true" : "false") << std::endl
        << "Scroll: " << map->getScroll() << std::endl;

        if (map->validate())
        {
            loadedValidMaps.push_back(map);
            std::cout << "Map is valid." << std::endl
            << SEPARATOR_LINE << std::endl;
        }
        else
        {
            std::cout << "**ERROR**: Map is NOT valid." << std::endl;
            delete map; // Clean up invalid map
            std::cout << SEPARATOR_LINE << std::endl;
            continue;
        }
        
        
    }
    

}