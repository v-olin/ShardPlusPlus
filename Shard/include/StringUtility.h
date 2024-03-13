#pragma once

#include <string>
#include <vector>

namespace Shard {

	class StringUtility {

	public:
		static std::vector<std::string> split(std::string s, std::string delimiter) {
			size_t pos_start = 0, pos_end, delim_len = delimiter.length();
			std::string token;
			std::vector<std::string> res;

			while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
				token = s.substr(pos_start, pos_end - pos_start);
				pos_start = pos_end + delim_len;
				res.push_back(token);
			}

			res.push_back(s.substr(pos_start));
			return res;
		}

		// Below implementations from: https://stackoverflow.com/a/25385766

		// trim from end of string (right)
		static inline std::string& rtrim(std::string& s, const char* t = "\t\n\r\f\v")
		{
			s.erase(s.find_last_not_of(t) + 1);
			return s;
		}

		// trim from beginning of string (left)
		static inline std::string& ltrim(std::string& s, const char* t = "\t\n\r\f\v")
		{
			s.erase(0, s.find_first_not_of(t));
			return s;
		}

		// trim from both ends of string (right then left)
		static inline std::string& trim(std::string& s, const char* t = "\t\n\r\f\v")
		{
			return ltrim(rtrim(s, t), t);
		}

		static std::string& replace(std::string& str, const std::string& oldStr, const std::string& newStr)
		{
			std::string::size_type pos = 0u;
			while ((pos = str.find(oldStr, pos)) != std::string::npos) {
				str.replace(pos, oldStr.length(), newStr);
				pos += newStr.length();
			}

			return str;
		}


	private:
		 StringUtility();
	};

}