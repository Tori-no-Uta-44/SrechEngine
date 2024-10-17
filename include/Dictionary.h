//
// Created by 李勃鋆 on 24-10-15.
//

#ifndef DICTIONARY_H
#define DICTIONARY_H
#include "func.h"

class Dictionary {
public:
	static Dictionary *getInstance();

	[[nodiscard]] const std::set<int>& getWordPosition(const std::string& word) const;

	[[nodiscard]] const std::set<int>& getWordPositionCh(const std::string& word) const;
	void init(const string &dictPath);//通过词典文件路径初始化词典

	vector<pair<string, int>> &getDict();//获取词典

	map<string, set<int>> &getIndexTable();//获取索引表

	vector<pair<string, int>> &getDictCh();//获取词典
	vector<pair<string, int>> &getDictALL();//获取词典
	map<string, set<int>> &getIndexTableCh();//获取索引表

	Dictionary(const Dictionary &other) = delete;

	Dictionary &operator=(const Dictionary &other) = delete;

	Dictionary(Dictionary &&other) = delete;

	Dictionary &operator=(Dictionary &&other) = delete;

private:
	Dictionary();

	void queryIndex();

	~Dictionary();

	static void destroyInstance();

	vector<pair<string, int>> _dict; //词典
	map<string, set<int>> _indexTable; //索引表
	vector<pair<string, int>> _dictCh; //中文词典
	map<string, set<int>> _indexTableCh; //中文索引表
	vector<pair<string, int>> _dictAll;
	static Dictionary *_dictionary;
	static std::once_flag _once_flag;
};
#endif //DICTIONARY_H
