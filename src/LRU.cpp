//
// Created by 李勃鋆 on 24-10-22.
//
#include "../include/LRU.h"

LRU::LRU() = default;

LRU::LRU(size_t capacity):
	_capacity(capacity) {
}

string LRU::get(const string &key) {
	auto it=_cacheMap.find(key);
	if(it==_cacheMap.end()) {
		return "";
	}
	_cacheList.splice(_cacheList.begin(),_cacheList,it->second);//把对应的节点移动到链表前面
	return it->second->second;
}

void LRU::put(const string &key, const string &value) {
	auto it=_cacheMap.find(key);
	if(it!=_cacheMap.end()) {
		_cacheList.splice(_cacheList.begin(),_cacheList,it->second);
		it->second->second = value;
	}else {
		if(_cacheList.size()==_capacity) {
			_cacheMap.erase(_cacheList.back().first);
			_cacheList.pop_back();
		}
		_cacheList.emplace_front(key,value);
		_cacheMap[key]=_cacheList.begin();
	}
}

void LRU::erase(const std::string &key) {
	auto it=_cacheMap.find(key);
	if(it!=_cacheMap.end()) {
		_cacheList.erase(it->second);
		_cacheMap.erase(it);
	}
}
