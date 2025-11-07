#include "Utils.h"

/**
 * THIS IS A FILE FOR DEFINING HELPER CONSTANTS AND FUNCTIONS
 */

extern const std::string MAP_SECTION_HEADER = "[Map]";
extern const std::string CONTINENT_SECTION_HEADER = "[Continents]";
extern const std::string TERRITORY_SECTION_HEADER = "[Territories]";
extern const std::string SEPARATOR_LINE =
	"================================================================";

std::vector<std::string> splitString(const std::string &str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		if (!token.empty()) {
			tokens.push_back(token);
		}
	}
	return tokens;
}