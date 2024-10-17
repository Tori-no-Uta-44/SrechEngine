//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef PAGELIB_H
#define PAGELIB_H
#include "Configuration.h"
#include "DirScanner.h"
#include "func.h"

class PageLib {
public:
	PageLib();

	PageLib(Configuration *conf, DirScanner *dirScanner);

	void create(); //创建网页库
	void store(); //存储网页库和位置偏移库
private:
	Configuration *_conf{};
	DirScanner *_dirScanner{};
	vector<string> _files;
	map<long, pair<long, long>> _offsetLib;
};

#endif //PAGELIB_H
