#include "BaseFunctionality.h"
#include "StringUtility.h"
#include "Bootstrap.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

namespace Shard {

	std::unordered_map<std::string, std::string> Shard::BaseFunctionality::readConfigFile(std::string file)
	{
		std::unordered_map<std::string, std::string> config_entries;
		std::string text = readFileAsString(file);

		std::vector<std::string> lines = StringUtility::split(text, "\n");
		std::vector<std::string> bits;
		std::string key, value;

		for (auto& line : lines) {

			// Skip blank lines
			if (line.empty())
				continue;

			// Skip comments
			if (line[0] == '#')
				continue;

			bits = StringUtility::split(line, ":");
			key = StringUtility::trim(bits[0]);
			value = StringUtility::trim(bits[1]);

			value = StringUtility::replace(value, "%BASE_DIR%", Bootstrap::getBaseDir());

			config_entries[key] = value;
			std::cout << "Reading: " << key << " and " << value << "\n";

		}

		std::cout << std::endl;

		return config_entries;
	}

	std::string Shard::BaseFunctionality::readFileAsString(std::string file)
	{
		std::ifstream f(file);
		std::stringstream buffer;
		buffer << f.rdbuf();
		return buffer.str();
	}

}
