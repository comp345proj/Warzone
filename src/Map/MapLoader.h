#pragma once
#include <string>
#include "Map.h"

class MapLoader {
private:
    std::string* filename;
    
    bool convertStrToBool(const std::string& str);

    void getMapDetails(const std::string line,
                       bool& wrap, bool& warn,
                       std::string& author,
                       std::string& image,
                       std::string& scroll);

    void getContinentDetails(const std::string line,
                             std::string& name,
                             int& bonus);

public:
    MapLoader(const std::string& filename);
    ~MapLoader();
    
    Map* loadMap();
};