//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef CPPJIEBA_H
#define CPPJIEBA_H
#include "Configuration.h"
#include "SplitTool.h"
class CppJieba:public SplitTool{
public:
	CppJieba();
	vector<string> cut(const string &sentence) override;
	~CppJieba() override;
	Configuration & _conf;
};
#endif //CPPJIEBA_H
