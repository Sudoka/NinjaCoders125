#include "ConfigurationManager.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <assert.h>
#include <algorithm>

#define CONFIGFILEPATH "../config.ini"
#define USERCONFIGFILEPATH "../config_dev.ini"

#pragma region Helper Functions.

/* Split - Takes in a string, a delimiter, and a vector.
 * returns the vector with the string split by the delimiter.
 */
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

// Very simple local function to strip all spaces from a string.
void stripSpaces(string & s) {
	for(unsigned int i = 0; i < s.length(); i++) {
		if(s[i] == ' ') {
			s.erase(i,1);
			i--;
		}
	}
}

// Very simple local string compare function.
bool comp(string s, char * c, int length) {
	for(int i = 0; i < length; i++) {
		if(s.at(i) != c[i]) {
			return false;
		}
	}
	return true;
}

#pragma endregion split, stripSpaces, and comp[are]

// Constructor - zeros out memory and sets initialized to false.
ConfigurationManager::ConfigurationManager() {
	SecureZeroMemory(&keytable, sizeof(keytable));
	SecureZeroMemory(&valuetable, sizeof(valuetable));
	SecureZeroMemory(&keylen, sizeof(keylen));
	SecureZeroMemory(&valuelen, sizeof(valuelen));
	initialized = false;
}

// Initializes the Configuration Manager. Parses the file and stores data.
void ConfigurationManager::initializefile(char * filepath) {
	ifstream conf(filepath, ios::in);
	string line;
	int x = 0;
	vector<string> chunk;
	if(conf.is_open()) {
		// cout << "Begin Configuration File Parsing..." << endl;
		while(conf.good()) {
			getline(conf,line);
			if(!(line.length() < 2) && !(line.at(0) == '#')) {
				// cout << "Parsing line: " << line << endl;
				chunk.clear();
				split(line, '=', chunk);
				// cout << "Number of Chunks: " << chunk.size() << endl;
				// Assert. If the configuration file fails, it should fail hard.
                printf("Chunk size = %d, len = %d (%s)\n", chunk.size(), line.length(), line.c_str());
				assert(chunk.size() == 2 && "Config file must have 1 key=value pair per line. All spaces are ignored. Comment with leading # symbol(no spaces before #).");
				string key = chunk[0];
				string value = chunk[1];
				// cout << "Line split by = 1) " << key << " with length :" << key.length() << endl;
				// cout << "Line split by = 2) " << value << " with length :" << value.length() << endl;
				stripSpaces(key);
				stripSpaces(value);
				// cout << "Line split, trimmed, and stored 1) " << key << " with length : " << key.length() << endl;
				// cout << "Line split, trimmed, and stored 2) " << value << " with length : " << value.length() << endl;
				memcpy(&(keytable[x][0]), key.c_str(), key.size());
				memcpy(&(valuetable[x][0]), value.c_str(), value.size());
				keylen[x] = key.size();
				valuelen[x] = value.size();

printf(__FILE__" %d: (k,v) = (%s,%s)\n", __LINE__, &(keytable[x][0]), &(valuetable[x][0]));


				x++;
			}
		}
		conf.close();
	}
}

// Code blatantly yoinked from the interwebs. Source: http://www.gamedev.net/topic/399558-how-to-copy-a-file-in-c/
int CopyFileHelper(string initialFilePath, string outputFilePath)
{
	ifstream initialFile(initialFilePath.c_str(), ios::in|ios::binary);
	ofstream outputFile(outputFilePath.c_str(), ios::out|ios::binary);
	//defines the size of the buffer
	initialFile.seekg(0, ios::end);
	long fileSize = (long)initialFile.tellg();
	//Requests the buffer of the predefined size

	//As long as both the input and output files are open...
	if(initialFile.is_open() && outputFile.is_open())
	{
		short * buffer = new short[fileSize/2];
		//Determine the file's size
		//Then starts from the beginning
		initialFile.seekg(0, ios::beg);
		//Then read enough of the file to fill the buffer
		initialFile.read((char*)buffer, fileSize);
		//And then write out all that was read
		outputFile.write((char*)buffer, fileSize);
		delete[] buffer;
	}
	//If there were any problems with the copying process, let the user know
	else if(!outputFile.is_open())
	{
		cout<<"I couldn't open "<<outputFilePath<<" for copying!\n";
		return 0;
	}
	else if(!initialFile.is_open())
	{
		cout<<"I couldn't open "<<initialFilePath<<" for copying!\n";
		return 0;
	}
		
	initialFile.close();
	outputFile.close();

	return 1;
}

// Initializes the static Configuration Manager.
void ConfigurationManager::init() {
	ifstream ifile(USERCONFIGFILEPATH);
	if (!ifile) {	// If config.user doesn't exist, we should copy config.ini and make one.
		ifile.close();
		CopyFileHelper(CONFIGFILEPATH, USERCONFIGFILEPATH);
	}
	initializefile(USERCONFIGFILEPATH);
}

// Fetches the singleton Configuration Manager object
ConfigurationManager * ConfigurationManager::get() {
	static ConfigurationManager ConfigManager;
	if (ConfigManager.initialized == false) {
		ConfigManager.init();
		ConfigManager.initialized = true;
	}
	return &ConfigManager;
}

#pragma region Functions to find a configuration value given a key

// Looks for a key and returns the value.
char * ConfigurationManager::find_config(string key) {
	return &(valuetable[find_config_index(key)][0]);
}

int ConfigurationManager::find_config_index(string key) {
	int strlength = key.size();
	for(int i = 0; i < MAX_CONFIG_ITEMS; i++) {
		bool a = keylen[i] == strlength;
		bool b = comp(key, &(keytable[i][0]), strlength);
		if(a && b) {
			return i;
		}
	}
	cout << key << endl;
	assert(false && "Could not find configuration key specified. DELETE YOUR Config_dev.ini FILE TO USE THE LATEST CONFIGS!");
	return NULL;
}

string ConfigurationManager::find_config_as_string(string key) {
	int index = find_config_index(key);
	return string((const char *)&(valuetable[index][0]), valuelen[index]);
}

int ConfigurationManager::find_config_as_int(string key) {
	int index = find_config_index(key);
	return atoi(&(valuetable[index][0]));
}

float ConfigurationManager::find_config_as_float(string key) {
	int index = find_config_index(key);
	return (float)atof(&(valuetable[index][0]));
}

bool ConfigurationManager::find_config_as_bool(string key) {
	int index = find_config_index(key);
	string boolean((const char *)&(valuetable[index][0]), valuelen[index]);
	std::transform(boolean.begin(), boolean.end(), boolean.begin(), ::tolower);
	if(comp(boolean, "true", 4)) {
		return true;
	}
	if(comp(boolean, "false", 5)) {
		return false;
	}
	assert(false && "Invalid Configuration value as boolean.");
	return false;
}

Vec3f ConfigurationManager::find_config_as_point(string key) {
	return parseAsPoint(find_config_as_string(key));
}

Vec3f ConfigurationManager::parseAsPoint(string value) {
	vector<string> chunk;
	split(value, ',', chunk);
	assert(chunk.size() == 3 && "Config Error: Points are represented as <floatx> , <floaty> , <floatz>. Spaces will ignored.");
	string x = chunk[0], y = chunk[1], z = chunk[2];
	stripSpaces(x); stripSpaces(y); stripSpaces(z);
	float fx = (float)atof(x.c_str()), fy = (float)atof(y.c_str()), fz = (float)atof(z.c_str());
	return Vec3f(fx, fy, fz);
}

Vec4f ConfigurationManager::parseAsVec4f(string value) {
	vector<string> chunk;
	split(value, ',', chunk);
	assert(chunk.size() == 4 && "Config Error: Vec4f's are represented as <floatx> , <floaty> , <floatz>, <floatw>. Spaces will ignored.");
	string x = chunk[0], y = chunk[1], z = chunk[2], w = chunk[3];
	stripSpaces(x); stripSpaces(y); stripSpaces(z);
	float fx = (float)atof(x.c_str()), fy = (float)atof(y.c_str()), fz = (float)atof(z.c_str()), fw = (float)atof(w.c_str());
	return Vec4f(fx, fy, fz, fw);
}

vector<pair<Point_t, Quat_t>> ConfigurationManager::find_config_as_places(string key) {
	vector<pair<Point_t, Quat_t>> result;
	//result.push_back(pair<Vec3f, Vec3f>(Vec3f(-20, 100, 300), Vec3f()));
	//result.push_back(make_pair(Point_t(-20, 100, -300), Rot_t((float)M_PI,0,0)));
	//return result;
	vector<string> pairs;
	string point = find_config_as_string(key);
	split(point, ';', pairs);

	// Check format
	assert(pairs.size() > 1 && "Config Error: Places vector is represented as place ; place ; place ; <(o^-^o)>. Spaces will be ignored.");
	assert(pairs.back() == "<(o^-^o)>" && "Config Error: Places vector end token should be a kirby <(o^-^o)>");

	int index = 0;

	// Kirby is our end token, deal with it
	// http://i.imgur.com/dvZzy.gif
	while (pairs[index] != "<(o^-^o)>")
	{
		vector<string> currPair;
		split(pairs[index], ':', currPair);

		assert(currPair.size() == 2 && "Config Error: Places are represented as position : rotation. Spaces will be ignored.");

		Point_t pos = parseAsPoint(currPair[0]);

		Vec4f v = parseAsVec4f(currPair[1]);
		Vec3f axis = Vec3f(v.x, v.y, v.z);
		Quat_t rot = Quat_t(axis, v.w);
		result.push_back(pair<Point_t, Quat_t>(pos, rot));
		//result[pos] = rot;

		index++;
	}

	return result;
}

Box ConfigurationManager::find_config_as_box(string key) {
	vector<string> chunk;
	string box = find_config_as_string(key);
	split(box, ',', chunk);
	assert(chunk.size() == 6 && "Config Error: Points are represented as <floatx> , <floaty> , <floatz> , <floatw> , <floath> , <floatl>. Spaces will ignored.");
	string x = chunk[0], y = chunk[1], z = chunk[2],
		   w = chunk[3], h = chunk[4], l = chunk[5];
	stripSpaces(x); stripSpaces(y); stripSpaces(z);
	stripSpaces(w); stripSpaces(h); stripSpaces(l);
	float fx = (float)atof(x.c_str()),
		  fy = (float)atof(y.c_str()),
		  fz = (float)atof(z.c_str()),
		  fw = (float)atof(w.c_str()),
		  fh = (float)atof(h.c_str()),
		  fl = (float)atof(l.c_str());
	return Box(fx, fy, fz, fw, fh, fl);
}

#pragma endregion
