//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef SPLITTOOL_H
#define SPLITTOOL_H
#include "func.h"

class SplitTool {
public:
	SplitTool();

	virtual vector<string> cut(const string &sentence) = 0;

	virtual ~SplitTool();
};
#endif // SPLITTOOL_H
