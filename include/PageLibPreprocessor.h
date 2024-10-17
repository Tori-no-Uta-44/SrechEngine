//
// Created by 李勃鋆 on 24-10-14.
//

#ifndef PAGELIBPREPROCESSOR_H
#define PAGELIBPREPROCESSOR_H
#include "Configuration.h"
#include "CppJieba.h"
#include "func.h"
#include "tinyxml2.h"
using namespace tinyxml2;

class PageLibPreprocessor {
public:
	struct WordInfo {
		std::string word;       // 单词
		string articleId;          // 文章编号
		long frequency;          // 在该文章中出现的频率
		long articleCount;       // 出现该单词的文章数
	};

	explicit PageLibPreprocessor(Configuration *conf, CppJieba *_jieba);

	void doProcess(long offset, long length); //执行预处理

	void readInfoFromFile(); //根据配置信息读取网页库和位置偏移库的内容

	void cutRedundantPages(); //对冗余的网页进行分词于去停用词

	void buildInvertIndexTable(); //创建倒排索引表

	void storeOnDisk(); //将经过预处理之后的网页库、位置偏移库和倒排索引表写回到磁盘上

	void addWord(const std::string& word, const string& articleId, long frequency, std::unordered_map<std::string, std::vector<WordInfo>>& wordMap);
private:

	typedef struct RssItem {
		string docid;
		string title;
		string link;
		string pubDate;
		string description;
	} RssItem;

	// 使用unordered_map进行存储，以便根据单词快速查找对应的信息
	std::unordered_map<std::string, std::vector<WordInfo>> _wordMap;
	vector<string> _lines;
	vector<RssItem> _pageLib;
	Configuration *_conf;
	CppJieba *_jieba;
	unordered_set<string> _stopWordList;
	unordered_map<int, pair<int, int>> _offsetLib;
	unordered_map<string, vector<pair<string, double>>> _invertIndexTable;

};
#endif //PAGELIBPREPROCESSOR_H
