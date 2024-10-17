//
// Created by 李勃鋆 on 24-10-12.
//

#include "../include/Configuration.h"
Configuration *Configuration::_configuration = nullptr;
std::once_flag Configuration::_once_flag;

Configuration *Configuration::getInstance() {
	call_once(_once_flag, []() {
		_configuration = new Configuration("../conf/Jieba.json");
		atexit(destroyInstance);
	});
	return _configuration;
}

map<string, string> &Configuration::getConfigMap() {
	ifstream inFile(_filepath);
	if (!inFile.is_open()) {
		std::cerr << "无法打开文件!" << std::endl;
		inFile.close();
	}
	nlohmann::json jsons;
	inFile >> jsons;
	for (auto &[key,value] : jsons.items()) {
		if (value.is_string()) {
			_configMap[key] = value.get<string>();
		}
	}
	inFile.close();
	return _configMap;
}

unordered_set<string> Configuration::getStopWordList() {
	DirScanner dir_scanner;
	dir_scanner.traverse("/root/a/infomation/txt/stop");
	unordered_set<string> _dict_stopEn;
	unordered_set<string> _dict_stopCh;
	vector<string> stopFiles = dir_scanner.files();
	loadStopWords(stopFiles[0], _dict_stopEn);
	loadStopWordsCh(stopFiles[1], _dict_stopCh);

	_stopWordList=_dict_stopEn;
	_stopWordList.insert(_dict_stopCh.begin(),_dict_stopCh.end());
	return _stopWordList;
}

void Configuration::loadStopWords(const string &filepath, unordered_set<string> &stopWordSet) {
	ifstream stream(filepath);
	if (!stream.is_open()) {
		std::cerr << "无法打开文件: " << filepath << std::endl;
		stream.close();
		return;
	}
	string line;
	while (getline(stream, line)) {
		istringstream buffer(line);
		string data;
		while (buffer >> data) {
			string result;
			result.reserve(line.size());
			bool lastWasAlpha = false;
			for (const char &ch : line) {
				if (isalpha(ch)) {
					result += static_cast<char>(tolower(ch));
					lastWasAlpha = true;
				}
				else {
					if (lastWasAlpha) {
						result += ' ';
						lastWasAlpha = false;
					}
				}
			}
			if (!result.empty() && result.back() == ' ') {
				result.pop_back();
			}
			if (!result.empty()) {
				stopWordSet.insert(result);
			}
		}
	}
	stream.close();
}

void Configuration::loadStopWordsCh(const string &filepath, unordered_set<string> &stopWordSet) {
	ifstream stream(filepath);
	if (!stream.is_open()) {
		std::cerr << "无法打开文件: " << filepath << std::endl;
		stream.close();
		return;
	}
	string line;
	while (getline(stream, line)) {
		// 移除行尾的 '\r' 字符
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}

		// 将处理后的行插入到 stopWordSet 中
		if (!line.empty()) {
			stopWordSet.insert(line);
		}
	}
	stream.close();
}

Configuration::Configuration(string filepath):
	_filepath(std::move(filepath)) {

}

Configuration::~Configuration() = default;

void Configuration::destroyInstance() {
	if (_configuration != nullptr) {
		delete _configuration;
		_configuration = nullptr;
	}
}
