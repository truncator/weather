#include "Config.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <sstream>

namespace Config
{
	void WriteFile()
	{
		// Get the config directory.
		std::string directory = GetConfigDirectory();

		// Create the config directory if it doesn't exist.
		if (!DirectoryExists(directory))
			mkdir(directory.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);

		std::ofstream config(directory + "config");

		if (!config)
		{
			printf("Failed to write config file to \'%s\'.\n", directory.c_str());
			return;
		}

		// TODO

		config.close();
	}

	ConfigVars ReadFile()
	{
		ConfigVars vars;

		// Get the config directory.
		std::string directory = GetConfigDirectory();

		// Open the config file for reading.
		std::ifstream config(directory + "config");

		if (!config)
		{
			printf("Failed to load config file from \'%s\'.\n", directory.c_str());
			return vars;
		}

		std::string line;

		// Read whole lines one-by-one.
		while (std::getline(config, line))
		{
			// Ignore commented lines.
			if (line.empty() || line[0] == '#')
				continue;

			std::istringstream parser(line);
			std::string key;

			// Scan until an '=' is found.
			if (std::getline(parser, key, '='))
			{
				std::string value;

				// Construct a key=value pair.
				if (std::getline(parser, value))
					vars.emplace(key, value);
			}
		}

		return vars;
	}
}

std::string GetHomeDirectory()
{
	const char* home = getenv("HOME");

	if (home == nullptr)
		return "~";
	else
		return home;
}

std::string GetConfigDirectory()
{
	return GetHomeDirectory() + "/.config/weather/";
}

bool DirectoryExists(std::string directory)
{
	struct stat info;
	return (stat(directory.c_str(), &info) == 0) && S_ISDIR(info.st_mode);
}

bool ToBoolean(std::string value)
{
	return value == "true" || ToInteger(value) != 0;
}

int ToInteger(std::string value)
{
	return std::strtol(value.c_str(), nullptr, 10);
}
