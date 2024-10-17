//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef CPPJIEBA_H
#define CPPJIEBA_H
#include "Configuration.h"
#include "Simhasher.hpp"
#include "SplitTool.h"
#include "cppjieba/Jieba.hpp"
using namespace simhash;
class CppJieba:public SplitTool{
public:
	CppJieba();
	vector<string> cut(const string &sentence) override;
	bool complete(const string& data1,const string& data2);
	~CppJieba() override;
private:
	Configuration *_conf;
	Jieba _jieba;
	Simhasher _simhasher;
};
#endif //CPPJIEBA_H
