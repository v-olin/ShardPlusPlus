#pragma once

#include <string>
#include <iostream>

namespace Shard {
	enum  LoggerLevel {
		LOG_LEVEL_NONE = 0,
		LOG_LEVEL_FATAL = 1,
		LOG_LEVEL_ERROR = 2,
		LOG_LEVEL_WARNING = 3,
		LOG_LEVEL_ALL = 4
	};

	class Logger {
	public:
		static  void setLoggerLevel(LoggerLevel new_level);
		static  void log(std::string log, LoggerLevel level);
		static  void log(const char* log);
			
	private:
		LoggerLevel logg_level = LOG_LEVEL_ALL;

		static Logger& getInstance() {
			static Logger logger;
			return logger;
		}
	};
}