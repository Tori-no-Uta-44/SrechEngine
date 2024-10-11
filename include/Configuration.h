//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "func.h"
class Configuration {
public:
	map<string, string> &getConfigMap();
	set<string> getStopWordList();

	Configuration(const Configuration &other) = delete;
	Configuration &operator=(const Configuration &other) = delete;
	Configuration(Configuration &&other) = delete;
	Configuration &operator=(Configuration &&other) = delete;

private:
	explicit Configuration(const string &filepath);
	~Configuration();
	static void destroyInstance();
	string _filepath;
	map<string, string> _configMap;
	set<string> _stopWordList;
	static Configuration *_configuration;
	static std::once_flag _once_flag;
};
#endif // CONFIGURATION_H
