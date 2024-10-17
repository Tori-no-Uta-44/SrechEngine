//
// Created by 李勃鋆 on 24-10-12.
//
#include "../include/CppJieba.h"

#include "../include/CppJieba.h"

CppJieba::CppJieba() :
	_conf(Configuration::getInstance()),
	_jieba(_conf->getConfigMap()["DICT_PATH"], _conf->getConfigMap()["HMM_PATH"],
	       _conf->getConfigMap()["USER_DICT_PATH"], _conf->getConfigMap()["IDF_PATH"],
	       _conf->getConfigMap()["STOP_WORD_PATH"]),
	_simhasher(_conf->getConfigMap()["DICT_PATH"], _conf->getConfigMap()["HMM_PATH"],
	           _conf->getConfigMap()["IDF_PATH"], _conf->getConfigMap()["STOP_WORD_PATH"]) {
}

std::vector<std::string> CppJieba::cut(const std::string &sentence) {
	std::vector<std::string> words;
	_jieba.Cut(sentence, words);
	return words;
}

bool CppJieba::complete(const std::string &data1, const std::string &data2) {
	if (data1.empty() || data2.empty()) {
		return false;
	}
	uint64_t v64_1, v64_2;
	size_t topN = 5;
	_simhasher.make(data1, topN, v64_1);
	_simhasher.make(data2, topN, v64_2);
	return Simhasher::isEqual(v64_1, v64_2, 5);
}


CppJieba::~CppJieba() {
	std::cout << "CppJieba destructor" << std::endl;
}
