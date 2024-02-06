// BaseFunctionality.h - Description
#pragma once

#include <string>
#include <unordered_map>

namespace Shard {
	// Singleton pattern taken from: https://stackoverflow.com/a/1008289
	class BaseFunctionality {
	public:
		static BaseFunctionality& getInstance()
		{
			static BaseFunctionality instance;
			return instance;
		}

		BaseFunctionality(BaseFunctionality const&) = delete;
		void operator=(BaseFunctionality const&) = delete;

		std::unordered_map<std::string, std::string> readConfigFile(std::string file);

	private:
		BaseFunctionality() {};

		std::string readFileAsString(std::string file);
	};

}