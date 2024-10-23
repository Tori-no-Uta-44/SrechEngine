//
// Created by 李勃鋆 on 24-10-23.
//

#ifndef SUBLRUCACHE_H
#define SUBLRUCACHE_H
#include "func.h"

class SubLRUCache {
public:
	explicit SubLRUCache(size_t capacity);

	std::string get(const std::string &key);

	void put(const std::string &key, const std::string &value);

	void erase(const std::string &key);

	[[nodiscard]] vector<pair<string, string>> get_inserts() const;

	[[nodiscard]] vector<pair<string, string>> get_updates() const;

	[[nodiscard]] vector<string> get_deletes() const;

	void clearRecords();

private:
	void recordInsert(const string &key, const string &value);

	void recordUpdate(const string &key, const string &value);

	void recordDelete(const string &key);

	size_t _capacity = 0;
	list<pair<string, string>> _cacheList; // 键值对链表
	unordered_map<string, list<pair<string, string>>::iterator> _cacheMap; //存储的是链表节点的迭代器
	vector<pair<string, string>> _inserts;
	vector<pair<string, string>> _updates;
	vector<string> _deletes;
};
#endif //SUBLRUCACHE_H
