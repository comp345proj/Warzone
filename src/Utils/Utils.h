#pragma once

#include <string>
#include <sstream>
#include <vector>

extern const std::string MAP_SECTION_HEADER;
extern const std::string CONTINENT_SECTION_HEADER;
extern const std::string TERRITORY_SECTION_HEADER;
extern const std::string SEPARATOR_LINE;

std::vector<std::string> splitString(const std::string &str, char delimiter);