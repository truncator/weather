#include <unistd.h>
#include <cstdio>
#include <string.h>
#include <fstream>
#include <sstream>

#include "Config.hpp"
#include "json/json.h"

bool FetchWeather(std::string location, bool metric, std::string& raw_data)
{
	// Construct an API request to OpenWeatherMap.org.
	std::string api_request = "http://api.openweathermap.org/data/2.5/find?q=";
	api_request += location;
	api_request += std::string("&units=") + (metric ? "metric" : "imperial");
	api_request += "&mode=json";

	// Fetch the raw data.
	std::string raw_data_filename = "raw_data.json";
	system(("wget -q -O " + raw_data_filename + " \"" + api_request + "\"").c_str());

	// Sleep for 300 ms to allow wget to finish its request.
	usleep(300 * 1000);

	std::ifstream is(raw_data_filename);

	// Read the contents of the raw data into a string.
	std::stringstream buffer;
	buffer << is.rdbuf();

	raw_data = buffer.str();

	is.close();

	// Delete the raw data file.
	system(("rm " + raw_data_filename).c_str());

	// A bad API request returns a small JSON file.
	if (raw_data.size() <= 54)
		return false;

	return true;
}

void PrintWeather(const std::string& raw_data)
{
	// Construct jsoncpp helpers.
	Json::Value root;
	Json::Reader reader;

	// Attempt to parse the raw data.
	if(!reader.parse(raw_data, root))
		printf("Failed to parse weather data.\n");

	Json::Value list = root["list"][0];

	// Look up weather parameters.
	std::string city_name = list["name"].asString();
	std::string country_name = list["sys"]["country"].asString();
	std::string temperature = list["main"]["temp"].asString();
	std::string humidity = list["main"]["humidity"].asString();
	std::string wind_speed = list["wind"]["speed"].asString();
	std::string cloud_cover = list["clouds"]["all"].asString();
	std::string weather_status = list["weather"][0]["main"].asString();
	std::string weather_description = list["weather"][0]["description"].asString();

	// Print the data.
	printf("Weather for %s, %s:\n", city_name.c_str(), country_name.c_str());
	printf("  Currently %s degrees at %s%s humidity.\n", temperature.c_str(), humidity.c_str(), "%");
	printf("  Conditions are %s with a wind speed of %s mph.\n", weather_description.c_str(), wind_speed.c_str());
}

void PrintUsage()
{
	printf("Usage: weather [zipcode] [city, country]\n");
	// TODO: --help
}

int main(int argc, char** argv)
{
	// Look up config variables.
	ConfigVars config = Config::ReadFile();
	int zipcode = ToInteger(config.find("zipcode")->second);
	bool metric = ToBoolean(config.find("metric")->second);

	std::string city_name;
	std::string country_name;
	std::string location;

	// Handle input arguments.
	if (argc == 2)
	{
		// Attempt to convert the argument into an integer.
		int zip = ToInteger(argv[1]);

		if (zip != 0)
		{
			// Success! Argument is a number.
			zipcode = ToInteger(argv[1]);
		}
		else
		{
			PrintUsage();
			return 1;
		}
	}
	else if (argc == 3)
	{
		city_name = argv[1];
		country_name = argv[2];
	}
	else if (argc > 3)
	{
		PrintUsage();
		return 1;
	}

	// Set the location to the input arguments.
	if (!city_name.empty() && !country_name.empty())
		location = city_name + "," + country_name;
	else
		location = std::to_string(zipcode);

	// Fetch the weather from the input location.
	std::string raw_data;
	if (!FetchWeather(location, metric, raw_data))
	{
		PrintUsage();
		return 1;
	}

	// Print the weather output from the raw JSON data.
	PrintWeather(raw_data);

	return 0;
}
