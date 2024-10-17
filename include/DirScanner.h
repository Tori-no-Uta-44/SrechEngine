//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef DIRSCANNER_H
#define DIRSCANNER_H
#include "func.h"

class DirScanner {
public:
	DirScanner();

	void operator()(const string &dirname); //重载函数调用运算符，调用traverse 函数

	void traverse(const string &dirname); //获取某一目录下的所有文件
	vector<string> &files();

private:
	stack<string> _directories;
	vector<string> _files;
};


#endif // DIRSCANNER_H
