#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace engine{
namespace utils{
        
    static std::string file_extension(const std::string& file_name)
    {
        const auto dot_pos = file_name.find_last_of('.');
        const auto extension_size = file_name.size() - dot_pos;
        std::string extension = file_name.substr(dot_pos + 1, extension_size);
        return extension;
    }

	static std::string read_file(const std::string &file_path)
	{
		std::fstream file_stream;
		file_stream.open(file_path);

		if(!file_stream.is_open())
		{
			return "Could not open file.";
		}


		std::string line;
		std::stringstream content_stream;
		while(std::getline(file_stream, line))
		{
			content_stream << line << "\n";
		}

		auto content = content_stream.str();
		if(content.empty())
			return "Something went wrong.";

		return content;
	}

	static std::vector<std::string> read_lines(const std::string &file_path)
	{
		std::ifstream stream(file_path);

		std::string line;
		std::vector<std::string> lines;
		while(getline(stream, line))
		{
			auto s = line + "\n";
			lines.push_back(s);
		}

		return lines;
	}

	static std::string read_file_c(const char* file_path)
	{
		FILE *file = fopen(file_path, "rt");
		fseek(file, 0, SEEK_END);
		const unsigned long length = ftell(file);
		char *data = new char[length + 1];
		memset(data, 0, length + 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, length, file);
		fclose(file);

		std::string result(data);
		delete[] data;
		return result;
	}

}
}
