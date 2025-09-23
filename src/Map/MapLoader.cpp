#include "MapLoader.h"
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <iostream>

// ---------- trim helpers -----------------
namespace {
// unnamed namespace = internal linkage, same effect as `static`
void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
				return !std::isspace(ch);
			}));
}

void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
						 [](unsigned char ch) { return !std::isspace(ch); })
				.base(),
			s.end());
}

void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}
}

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

    if (!file.is_open())
    {
        std::cerr << "Could not open the file: " << *filename << std::endl;
        return nullptr;
    }

    std::string mapName = filename->substr(0, filename->size() - 4);

}

