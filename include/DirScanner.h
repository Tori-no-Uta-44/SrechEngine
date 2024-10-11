//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef DIRSCANNER_H
#define DIRSCANNER_H
#include "func.h"
class DirScanner {
public:
	DirScanner();
	void operator()();
	vector<string> files();
	void traverse(const string &dirname);

private:
	vector<string> _files;
};


#endif // DIRSCANNER_H
