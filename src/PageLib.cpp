//
// Created by 李勃鋆 on 24-10-11.
//

#include "../include/PageLib.h"

PageLib::PageLib() = default;

PageLib::PageLib(Configuration *conf, DirScanner *dirScanner):
	_conf(conf),
	_dirScanner(dirScanner) {
	_dirScanner->traverse("/root/a/infomation/xml");
	_files = _dirScanner->files();
}

void PageLib::create() {
	std::ofstream writer("/root/a/SearchEngine/data/ripePage.txt", std::ios::out);
	if (!writer.is_open()) {
		std::cerr << "Failed to open the output file." << std::endl;
		writer.close();
		return;
	}

	int index = 0;
	for (const auto &file : _files) {
		std::ifstream stream(file);
		if (!stream.is_open()) {
			stream.close();
			std::cerr << "Failed to open the input file: " << file << std::endl;
			continue;
		}

		// 记录当前文件写入的起始位置
		long start = writer.tellp();

		// 写入文件内容
		std::string line;
		while (std::getline(stream, line)) {
			writer << line << '\n';
		}

		// 计算长度并存储偏移和长度
		long length = writer.tellp() - start;
		_offsetLib.emplace(index, std::make_pair(start, length));

		++index;
		stream.close();
	}
	writer.close();
}

void PageLib::store() {
	ofstream writer("/root/a/SearchEngine/data/offSetLib.txt", ios::app);
	if (!writer.is_open()) {
		cerr << "Failed to open the file." << endl;
		return;
	}
	for (auto &[index,length] : _offsetLib) {
		writer << index << " " << length.first << " " << length.second << endl;
	}
	writer.close();
}
