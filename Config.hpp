#pragma once

#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> ConfigVars;

namespace Config
{
	void WriteFile();

	// Return a map of config variables.
	ConfigVars ReadFile();
}

// Utility functions.
std::string GetHomeDirectory();
std::string GetConfigDirectory();
bool DirectoryExists(std::string directory);

// String conversions.
bool ToBoolean(std::string value);
int ToInteger(std::string value);
