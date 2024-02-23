#include "Logger.h"

namespace Shard {

	void Logger::setLoggerLevel(LoggerLevel new_level) {
		getInstance().logg_level = new_level;
	}

	void Logger::log(std::string msg, LoggerLevel level) {
		return;
		if (level <= getInstance().logg_level) {
			std::cout << msg << std::endl;
		}

		if (level == LoggerLevel::LOG_LEVEL_FATAL) {
			std::cout << "Stopping program to stop fucked up things from happening";
			// kill program, but don't close terminal window
			// this should dump a log-file but i don't care
			// enough to do IO right now...
			while (true) {};
		}
	}
	void Logger::log(const char* msg) {
		log(msg, getInstance().logg_level);
	}
}