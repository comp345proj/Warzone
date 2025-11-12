#include "MapDriver.h"
#include "Map.h"
#include "MapLoader.h"
#include "Utils/Utils.h"
#include <filesystem>
#include <iostream>
#include <vector>

void testLoadMaps() {
    std::cout << "\n=== Testing Map Loading and Validation ===\n" << std::endl;

    // Get the current working directory and construct paths
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path worldMap = currentPath / "res" / "World.map";
    std::filesystem::path invalidMap = currentPath / "res" / "invalid.map";
    std::filesystem::path disconnectedGraph =
        currentPath / "res" / "disconnected_graph.map";
    std::filesystem::path disconnectedContinent =
        currentPath / "res" / "disconnected_continent.map";

    std::vector<std::string> mapFiles = {
        worldMap.string(),
        invalidMap.string(),
        disconnectedGraph.string(),
        disconnectedContinent.string(),
    };

    std::vector<Map*> loadedValidMaps;

    for (int i = 0; i < mapFiles.size(); i++) {
        MapLoader loader(mapFiles[i]);
        Map* map = loader.loadMap();
        if (!map) {
            std::cout << SEPARATOR_LINE << std::endl;
            std::cerr << "Failed to load map from file: " << mapFiles[i]
                      << std::endl;
            continue;
        }

        std::cout << "Testing map file: " << map->getName() << std::endl;

        std::cout << "Map Details:" << std::endl;
        std::cout << "Author: " << map->getAuthor() << std::endl
                  << "Image: " << map->getImage() << std::endl
                  << "Wrap: " << (map->getWrap() ? "true" : "false")
                  << std::endl
                  << "Scroll: " << map->getScroll() << std::endl;

        if (map->validate()) {
            loadedValidMaps.push_back(map);
            std::cout << "Map is valid." << std::endl
                      << SEPARATOR_LINE << std::endl;
        } else {
            std::cout << "**ERROR**: Map is NOT valid." << std::endl;
            delete map; // Clean up invalid map
            std::cout << SEPARATOR_LINE << std::endl;
            continue;
        }
    }
}