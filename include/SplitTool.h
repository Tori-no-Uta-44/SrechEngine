//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef SPLITTOOL_H
#define SPLITTOOL_H
#include "func.h"

class SplitTool {
public:
	virtual vector<string> cut(const string &sentence) = 0;

	virtual ~SplitTool() = default;
};
#endif // SPLITTOOL_H
