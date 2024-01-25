#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <string>
#include <iostream>

using std::cout, std::endl;

namespace Shard {
	enum SHARD_API LoggerLevel {
		LOG_LEVEL_NONE = 0,
		LOG_LEVEL_ERROR = 1,
		LOG_LEVEL_WARNING = 2,
		LOG_LEVEL_ALL = 3
	};

	class Logger {
	public:
		static SHARD_API void setLoggerLevel(LoggerLevel new_level);
		static SHARD_API void log(std::string log, LoggerLevel level);
		static SHARD_API void log(std::string log);
			
	private:
		LoggerLevel logg_level = LOG_LEVEL_ALL;

		static Logger& getInstance() {
			static Logger logger;
			return logger;
		}
	};
}