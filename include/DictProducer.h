//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef DICTPRODUCER_H
#define DICTPRODUCER_H
#include "SplitTool.h"
#include "func.h"

class DictProducer {
public:
	DictProducer(const string &dir);
	DictProducer(const string &dir, SplitTool *splitTool);
	void buildEnDict();
	void buildCnDict();
	void storeDict(const char *filepath);
	void showFiles() const;
	void showDict()const;
	void getFiles();
	void pushDict(const string & word);
private:
	vector<string> _files;
	vector<pair<string, int>> _dict;
	SplitTool *_splitTool;
	map<string, set<int>> _index;
};

#endif // DICTPRODUCER_H
