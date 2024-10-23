//
// Created by 李勃鋆 on 24-10-22.
//

#ifndef LRU_H
#define LRU_H
#include "func.h"

class LRU {
public:
	LRU();

	explicit LRU(size_t capacity);

	string get(const string &key);

	void put(const string &key, const string &value);

	void erase(const std::string &key);

private:
	size_t _capacity = 0;
	list<pair<string, string>> _cacheList; // 键值对链表
	unordered_map<string, list<pair<string, string>>::iterator> _cacheMap; //存储的是链表节点的迭代器
};
#endif //LRU_H
