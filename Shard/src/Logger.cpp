#include "Logger.h"

namespace Shard {

	void Logger::setLoggerLevel(LoggerLevel new_level) {
		getInstance().logg_level = new_level;
	}

	void Logger::log(std::string msg, LoggerLevel level) {
		if (level <= getInstance().logg_level) {
			std::cout << msg << std::endl;
		}
	}
	void Logger::log(const char* msg) {
		log(msg, getInstance().logg_level);
	}
}