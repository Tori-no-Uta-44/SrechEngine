//
// Created by 李勃鋆 on 24-10-11.
//

#ifndef DICTPRODUCER_H
#define DICTPRODUCER_H
#include "SplitTool.h"
#include "func.h"
#include "Configuration.h"
#include "DirScanner.h"
#include "CppJieba.h"

class DictProducer {
public:
	explicit DictProducer(const string &dir);

	DictProducer(const string &dir, SplitTool *splitTool);

	void buildEnDict(const string &path);

	void buildCnDict(const string &path);

	void storeDict(const char *filepath); //字典索引

	void storeDictCh(const char *filepath);

	void showFiles() const;

	void showDict() const;

	void getFiles();

	void pushDict(); //字典写入文件


	void buildIndex(const map<string, int> &dict, map<string, set<int>> &ind);

	string dealLine(const string &line);

	size_t getByteNum_UTF8(const char byte);

private:
	string cleanText(const string &filePath);

	vector<string> _files; //文件夹里面的所有文件名
	map<string, int> _dict; //该单词出现的次数英文
	map<string, int> _dict_Ch; //该单词出现的次数英文
	unordered_set<string> _stopWordList;
	SplitTool *_splitTool{}; //切中文用
	map<string, set<int>> _index; //索引
	map<string, set<int>> _index_Ch; //索引
	vector<string> _lines; //O(1)存储该文件的内容，以便输出每个单词所在的原始行
};

#endif // DICTPRODUCER_H
