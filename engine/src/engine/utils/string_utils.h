#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace engine{
namespace utils{
    
	/// \brief
	static const char* find_token(const char* str, const std::string& token)
	{
		const char* t = str;
		while (t = strstr(t, token.c_str()))
		{
			bool left = str == t || isspace(t[-1]);
			bool right = !t[token.size()] || isspace(t[token.size()]);
			if (left && right)
				return t;

			t += token.size();
		}
		return nullptr;
	}

    /// \brief find_token string overload
	static const char* find_token(const std::string& string, const std::string& token)
	{
        return find_token(string.c_str(), token);
	}

	static std::vector<std::string> split_string(const std::string &string, const std::string& delimiters)
	{
		size_t start = 0;
		size_t end = string.find_first_of(delimiters);

		std::vector<std::string> result;

		while (end <= std::string::npos)
		{
			std::string token = string.substr(start, end - start);
			if (!token.empty())
				result.push_back(token);

			if (end == std::string::npos)
				break;

			start = end + 1;
			end = string.find_first_of(delimiters, start);
		}

		return result;
	}

	static std::vector<std::string> split_string(const std::string& string, const char delimiter)
	{
		return split_string(string, std::string(1, delimiter));
	}

    /// \brief 
    static std::vector<std::string> tokenize(const std::string& string)
	{
		return split_string(string, " \t\n");
	}
    
	static bool starts_with(const std::string& string, const std::string& start)
	{
		return string.find(start) == 0;
	}

    /// \brief Retrieves a full c++ statement from start of the string till next ; included.
	static std::string get_statement(const char* str, const char** out_position)
	{
		const char* end = strstr(str, ";");
		if (!end)
			return str;

		if (out_position)
			*out_position = end;
		uint32_t length = end - str + 1;
		return std::string(str, length);
	}
}
}
