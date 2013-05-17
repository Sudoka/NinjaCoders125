#pragma once

#include <string>
#include <vector>
#include "defs.h"
#include "Frame.h"

using namespace std;

#define MAX_CONFIG_ITEMS 200
#define MAX_KEY_LENGTH 32
#define MAX_VALUE_LENGTH 256

class ConfigurationManager
{
public:
	ConfigurationManager();
	// virtual ~ConfigurationManager();
	void initializefile(char * filepath);								// Initializes the Configuration Manager

	static ConfigurationManager * get();

	char* find_config(std::string config);
	string find_config_as_string(string key);
	int find_config_as_int(string key);
	float find_config_as_float(string key);
	bool find_config_as_bool(string key);
	Vec3f find_config_as_point(string key);
	Box   find_config_as_box(string key);
	vector<Frame> find_config_as_places(string key);

private:
	void init();

	bool initialized;										// Set to true when init() is called
	int  find_config_index(string config);
	char keytable[MAX_CONFIG_ITEMS][MAX_KEY_LENGTH];		// Table of keys.
	char valuetable[MAX_CONFIG_ITEMS][MAX_VALUE_LENGTH];	// Table of values.
	int  keylen[MAX_CONFIG_ITEMS];							// Number of characters of each key
	int  valuelen[MAX_CONFIG_ITEMS];						// Number of characters of each value

	// Parsers
	Vec3f parseAsPoint(string value);
	Vec4f parseAsVec4f(string value);
};
typedef ConfigurationManager CM;