//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "func.h"
#include "nlohmann/json.hpp"
#include "DirScanner.h"

class Configuration {
public:
	static Configuration *getInstance();

	map<string, string> &getConfigMap();

	unordered_set<string> getStopWordList();

	Configuration(const Configuration &other) = delete;

	Configuration &operator=(const Configuration &other) = delete;

	Configuration(Configuration &&other) = delete;

	Configuration &operator=(Configuration &&other) = delete;

private:
	explicit Configuration(string filepath);
	void loadStopWords(const string &filepath, unordered_set<string> &stopWordSet);
	void loadStopWordsCh(const string &filepath, unordered_set<string> &stopWordSet);
	~Configuration();

	static void destroyInstance();

	string _filepath;
	map<string, string> _configMap;
	unordered_set<string> _stopWordList;
	static Configuration *_configuration;
	static std::once_flag _once_flag;
};
#endif // CONFIGURATION_H
