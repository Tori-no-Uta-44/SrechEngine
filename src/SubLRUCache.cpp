//
// Created by 李勃鋆 on 24-10-23.
//
#include "../include/SubLRUCache.h"

SubLRUCache::SubLRUCache(size_t capacity):_capacity(capacity) {
}

std::string SubLRUCache::get(const std::string &key) {
	auto it=_cacheMap.find(key);
	if(it==_cacheMap.end()) {
		return "";
	}
	_cacheList.splice(_cacheList.begin(),_cacheList,it->second);
	return it->second->second;
}

void SubLRUCache::put(const std::string &key, const std::string &value) {
	auto it=_cacheMap.find(key);
	if(it!=_cacheMap.end()) {
		_cacheList.splice(_cacheList.begin(),_cacheList,it->second);
		it->second->second=value;
		recordUpdate(key,value);
	}else {
		if(_cacheList.size()==_capacity) {
			auto last=_cacheList.back();
			_cacheMap.erase(last.first);
			_cacheList.pop_back();
			recordInsert(key,value);
		}
	}
}

void SubLRUCache::erase(const std::string &key) {
	auto it=_cacheMap.find(key);
	if(it!=_cacheMap.end()) {
		recordDelete(key);
		_cacheList.erase(it->second);
		_cacheMap.erase(it);
	}
}

vector<pair<string, string>> SubLRUCache::get_inserts() const {
	return _inserts;
}

vector<pair<string, string>> SubLRUCache::get_updates() const {
	return _updates;
}

vector<string> SubLRUCache::get_deletes() const {
	return _deletes;
}

void SubLRUCache::clearRecords() {
	_inserts.clear();
	_updates.clear();
	_deletes.clear();
}

void SubLRUCache::recordInsert(const string &key, const string &value) {
	_inserts.emplace_back(key, value);
}

void SubLRUCache::recordUpdate(const string &key, const string &value) {
	_updates.emplace_back(key,value);
}

void SubLRUCache::recordDelete(const string &key) {
	_deletes.push_back(key);
}
