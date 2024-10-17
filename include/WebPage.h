//
// Created by 李勃鋆 on 24-10-13.
//

#ifndef WEBPAGE_H
#define WEBPAGE_H
#include "Configuration.h"
#include "CppJieba.h"
#include "func.h"
#include "tinyxml2.h"
using namespace tinyxml2;

class WebPage {
public:
	WebPage(const string &doc, Configuration &config, CppJieba &jieba);

	[[nodiscard]] int getDocId() const;

	[[nodiscard]] string getDoc() const;

	void processDoc(const string &filePath, const std::string &offsetFilePath, Configuration &config,
	                CppJieba &jieba); //对格式化文档进行处理
	static std::string removeHtmlTags(const std::string &html);

	void loadArticlesFromOffsets(const std::string &offsetFilePath);

	void parseRssByOffset(long offset, long length);

	void dump();

	void dump_new();

	friend bool operator==(const WebPage &lhs, const WebPage &rhs);

	friend bool operator <(const WebPage &lhs, const WebPage &rhs);

	typedef struct RssItem {
		string title;
		string link;
		string pubDate;
		string description;
	} RssItem;

	map<long, pair<long, long>> _offsetLib;
	map<long, pair<long, long>> _offsetLib_new;
	string _doc;
	Configuration &_config;
	CppJieba &_jieba;
	int _docId{};
	string _docTitle;
	string _docUrl;
	string _docContent;
	vector<RssItem> _rss;
	vector<RssItem> _rss_new;
	vector<string> _lines;
};
#endif //WEBPAGE_H
