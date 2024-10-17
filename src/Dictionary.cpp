//
// Created by 李勃鋆 on 24-10-15.
//
#include "../include/Dictionary.h"

Dictionary *Dictionary::_dictionary = nullptr;
std::once_flag Dictionary::_once_flag;

Dictionary *Dictionary::getInstance() {
	call_once(_once_flag, []() {
		_dictionary = new Dictionary();
		atexit(destroyInstance);
	});
	return _dictionary;
}

const std::set<int> &Dictionary::getWordPosition(const std::string &word) const {
	static const std::set<int> emptySet;
	auto it = _indexTable.find(word);
	return (it != _indexTable.end()) ? it->second : emptySet;
}

Dictionary::Dictionary() = default;

const std::set<int> &Dictionary::getWordPositionCh(const std::string &word) const {
	static const std::set<int> emptySet;
	auto it = _indexTableCh.find(word);
	return (it != _indexTableCh.end()) ? it->second : emptySet;
}

void Dictionary::init(const string &dictPath) {
	ifstream dict_en(dictPath + "dictionary.txt");
	if (!dict_en.is_open()) {
		cerr << "Failed to open the file." << endl;
		return;
	}
	ifstream dict_ch(dictPath + "dictionaryCh.txt");
	if (!dict_ch.is_open()) {
		cerr << "Failed to open the file." << endl;
		return;
	}
	ifstream index_en(dictPath + "dictionaryIndex.txt");
	if (!dict_ch.is_open()) {
		cerr << "Failed to open the file." << endl;
		return;
	}
	ifstream index_ch(dictPath + "dictionaryIndexCh.txt");
	if (!dict_ch.is_open()) {
		cerr << "Failed to open the file." << endl;
		return;
	}
	string line;
	while (getline(dict_en, line)) {
		istringstream iss(line);
		string word;
		int count;
		if (getline(iss, word, '|') && iss >> count) {
			_dict.emplace_back(word, count);
		}
	}

	line.clear();
	while (getline(dict_ch, line)) {
		istringstream iss(line);
		string word;
		int count;
		if (getline(iss, word, '|') && iss >> count) {
			_dictCh.emplace_back(word, count);
		}
	}

	line.clear();
	while (getline(index_en, line)) {
		istringstream iss(line);
		string key;
		if (getline(iss, key, ':')) {
			string numbers;
			if (getline(iss, numbers)) {
				istringstream numberStream(numbers);
				string number;
				while (getline(numberStream, number, ',')) {
					_indexTable[key].insert(stoi(number));
				}
			}
		}
	}

	line.clear();
	while (getline(index_ch, line)) {
		istringstream iss(line);
		string key;
		if (getline(iss, key, ':')) {
			string numbers;
			if (getline(iss, numbers)) {
				istringstream numberStream(numbers);
				string number;
				while (getline(numberStream, number, ',')) {
					_indexTableCh[key].insert(stoi(number));
				}
			}
		}
	}
	_dictAll = _dict; // 将 _dict 内容拷贝到 _dictAll
	_dictAll.insert(_dictAll.end(), _dictCh.begin(), _dictCh.end()); // 将 _dictCh 的内容追加到 _dictAll
	index_ch.close();
	index_en.close();
	dict_ch.close();
	dict_en.close();
}

vector<pair<string, int>> &Dictionary::getDict() {
	return _dict;
}

map<string, set<int>> &Dictionary::getIndexTable() {
	return _indexTable;
}

vector<pair<string, int>> &Dictionary::getDictCh() {
	return _dictCh;
}

map<string, set<int>> &Dictionary::getIndexTableCh() {
	return _indexTableCh;
}

void Dictionary::queryIndex() {
}

Dictionary::~Dictionary() = default;

void Dictionary::destroyInstance() {
	if (_dictionary != nullptr) {
		delete _dictionary;
		_dictionary = nullptr;
	}
}

vector<pair<string, int>> &Dictionary::getDictALL() {
	return _dictAll;
}
