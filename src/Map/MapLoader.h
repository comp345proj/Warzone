#pragma once
#include <string>
#include "Map.h"

class MapLoader {
private:
    std::string* filename;
    
    bool convertStrToBool(const std::string& str);

public:
    MapLoader(const std::string& filename);
    ~MapLoader();
    
    Map* loadMap();
};