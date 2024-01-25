// BaseFunctionality.h - Description
#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>
#include <unordered_map>

namespace Shard {

	// Singleton pattern taken from: https://stackoverflow.com/a/1008289

	class BaseFunctionality {

	public:
		static SHARD_API BaseFunctionality& getInstance()
		{
			static BaseFunctionality instance;
			return instance;
		}

		SHARD_API BaseFunctionality(BaseFunctionality const&) = delete;
		SHARD_API void operator=(BaseFunctionality const&) = delete;

		SHARD_API std::unordered_map<std::string, std::string> readConfigFile(std::string file);


	private:
		BaseFunctionality() {};

		std::string readFileAsString(std::string file);
	};

}