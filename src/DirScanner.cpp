//
// Created by 李勃鋆 on 24-10-11.
//

#include "../include/DirScanner.h"

DirScanner::DirScanner()=default;

void DirScanner::operator()(const string &dirname) {
	traverse(dirname);
}

vector<string>& DirScanner::files() {
	return _files;
}

void DirScanner::traverse(const string &dirname) {
	_directories.push(dirname);
	while (!_directories.empty()) {
		string currentDir = _directories.top();
		_directories.pop();
		DIR *_dir = opendir(currentDir.c_str());
		if (_dir == nullptr) {
			std::cerr << "无法打开目录: " << currentDir << "，错误代码：" << strerror(errno) << std::endl;
			continue;
		}
		dirent *entry;
		while ((entry = readdir(_dir)) != nullptr) {
			string name = entry->d_name;
			if (name == "." || name == "..") {
				continue;
			}
			string fullPath = currentDir;
			fullPath += "/";
			fullPath += name;
			if (entry->d_type == DT_REG) {
				// 普通文件
				_files.emplace_back(fullPath);
			}
			else if (entry->d_type == DT_DIR) {
				// 目录
				_directories.push(fullPath);
			}
		}
		closedir(_dir);
	}
}
